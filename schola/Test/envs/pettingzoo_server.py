# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
"""
PettingZoo Server for Schola RLlib Integration

This module provides gRPC servicers that wrap PettingZoo Parallel API environments,
enabling them to be used with Schola's RLlib integration (RayEnv and RayVecEnv).

Requirements:
    - pettingzoo (install with: pip install pettingzoo)
    - Any specific environment dependencies (e.g., pip install pettingzoo[mpe])

Supported Environments:
    - Any PettingZoo Parallel API environment
    - Examples: simple_spread_v3, pistonball_v6, pursuit_v4

Classes:
    - PettingZooToGymServiceServicer: Wraps a single PettingZoo environment
    - VecPettingZooToGymServiceServicer: Wraps multiple PettingZoo environments for vectorized execution

Example Usage:
    # Single environment
    servicer = PettingZooToGymServiceServicer('simple_spread_v3')

    # Multiple environments
    env_fns = [lambda: simple_spread_v3.parallel_env() for _ in range(4)]
    servicer = VecPettingZooToGymServiceServicer(env_fns)

For more examples, see test_rllib_pettingzoo.py
"""

import functools
import threading
from typing import Callable, List, Dict, Optional, Type
import numpy as np
from pettingzoo import ParallelEnv
from pettingzoo.utils import BaseWrapper, parallel_to_aec, aec_to_parallel
from schola.core.protocols.protobuf.deserialize import from_proto
from schola.core.protocols.protobuf.serialize import (
    space_to_proto,
    to_proto,
    make_generic,
    fill_generic,
)
from schola.generated.GymConnector_pb2_grpc import GymServiceServicer
from schola.generated.GymConnector_pb2 import *
from schola.generated.Definitions_pb2 import *
from schola.generated.StateUpdates_pb2 import *
from schola.generated.Spaces_pb2 import *
from schola.generated.Points_pb2 import *
from schola.generated.State_pb2 import *
import grpc
import traceback


def capture_traceback(func):
    """
    Capture traceback and move it to the body of the message so that it is forwarded to the client.
    """

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            raise Exception(traceback.format_exc())

    return wrapper


def wrap(
    env: ParallelEnv | Callable[..., ParallelEnv],
    wrappers: Optional[List[Type[BaseWrapper]]] = None,
) -> ParallelEnv:
    """Wrap a PettingZoo environment with optional wrappers."""
    if not isinstance(env, ParallelEnv):
        env = env()

    if wrappers:
        aec_env = parallel_to_aec(env)
        for wrapper in wrappers:
            aec_env = wrapper(aec_env)
        env = aec_to_parallel(aec_env)
    return env


class PettingZooToGymServiceServicer(GymServiceServicer):
    """
    gRPC servicer that wraps a single PettingZoo Parallel environment.
    Converts between PettingZoo's Parallel API and Schola's protobuf protocol.
    """

    def __init__(self, env_factory: Callable, wrappers: Optional[list] = None):
        """
        Initialize the PettingZoo servicer.

        Args:
            env_factory: A callable that returns a PettingZoo env
            wrappers: Optional list of wrapper classes to apply
        """
        self._env_factory = env_factory
        self._last_reset_obs = None
        self._last_reset_info = None
        self._env = None
        self._wrapper_classes = wrappers if wrappers else []

    @property
    def env(self) -> ParallelEnv:
        assert self._env is not None, "Environment not initialized"
        return self._env

    def UpdateState(self, request: StateUpdate, context) -> State:
        """Handle reset and step requests from the client."""
        msg_type = request.WhichOneof("update")

        if msg_type == "reset":
            # Reset the environment
            seed = (
                request.reset.environments[0].seed
                if request.reset.environments[0].HasField("seed")
                else None
            )
            options = (
                dict(request.reset.environments[0].options)
                if request.reset.environments[0].options
                else None
            )

            # PettingZoo Parallel API: reset() -> observations, infos
            self._last_reset_obs, self._last_reset_info = self.env.reset(
                seed=seed, options=options
            )

            # Convert to protobuf format
            agent_states = {}
            for agent_id, obs in self._last_reset_obs.items():
                obs_space = self.env.observation_space(agent_id)
                agent_states[agent_id] = InitialAgentState(
                    observations=make_generic(to_proto(obs_space, obs)),
                    info=self._last_reset_info.get(agent_id, {}),
                )

            response = State(
                initial_state=InitialState(
                    environment_states={
                        0: InitialEnvironmentState(agent_states=agent_states)
                    }
                )
            )
            return response

        elif msg_type == "step":
            # Extract actions for all agents
            actions = {}
            for agent_id, action_proto in request.step.environments[0].updates.items():
                actions[agent_id] = from_proto(action_proto)

            # PettingZoo Parallel API: step(actions) -> observations, rewards, terminations, truncations, infos
            observations, rewards, terminations, truncations, infos = self.env.step(
                actions
            )

            # Convert to protobuf format
            agent_states = {}
            for agent_id in observations.keys():
                obs_space = self.env.observation_space(agent_id)
                agent_states[agent_id] = AgentState(
                    observations=make_generic(
                        to_proto(obs_space, observations[agent_id])
                    ),
                    reward=float(rewards.get(agent_id, 0.0)),
                    terminated=bool(terminations.get(agent_id, False)),
                    truncated=bool(truncations.get(agent_id, False)),
                    info=infos.get(agent_id, {}),
                )

            return State(
                training_state=TrainingState(
                    environment_states=[EnvironmentState(agent_states=agent_states)]
                )
            )

        elif request.status == CLOSED:
            return State()
        elif request.status == ERROR:
            return State()
        else:
            raise ValueError("Invalid update message")

    def RequestInitialTrainingState(
        self, request: InitialStateRequest, context
    ) -> InitialState:
        """Request the initial state after reset."""
        agent_states = {}
        assert (
            self._last_reset_obs is not None
        ), "Last reset observations are not initialized"
        for agent_id, obs in self._last_reset_obs.items():
            obs_space = self.env.observation_space(agent_id)
            agent_states[agent_id] = InitialAgentState(
                observations=make_generic(to_proto(obs_space, obs)),
                info=self._last_reset_info.get(agent_id, {}),
            )

        return InitialState(
            environment_states={0: InitialEnvironmentState(agent_states=agent_states)}
        )

    def RequestTrainingDefinition(
        self, request: TrainingDefinitionRequest, context
    ) -> TrainingDefinition:
        """Define the observation and action spaces for all agents."""
        agent_definitions = {}

        # PettingZoo provides observation_space(agent) and action_space(agent) methods
        for agent_id in self.env.possible_agents:
            obs_space = self.env.observation_space(agent_id)
            action_space = self.env.action_space(agent_id)

            agent_definitions[agent_id] = AgentDefinition(
                obs_space=make_generic(space_to_proto(obs_space)),
                action_space=make_generic(space_to_proto(action_space)),
            )

        return TrainingDefinition(
            environment_definitions=[
                EnvironmentDefinition(agent_definitions=agent_definitions)
            ]
        )

    def StartGymConnector(
        self, request: GymConnectorStartRequest, context
    ) -> GymConnectorStartResponse:
        """Initialize the environment."""
        self._env = wrap(self._env_factory, self._wrapper_classes)
        return GymConnectorStartResponse()

    def close(self):
        """Explicitly close the environment. Must be called from the main thread for pygame-backed envs."""
        if self._env is not None:
            self._env.close()
            self._env = None

    def __del__(self):
        # pygame's SDL is not thread-safe; calling display teardown from a background
        # thread (e.g. gRPC ThreadPoolExecutor or GC) causes a segfault. Only close
        # from the main thread; explicit close() handles cleanup in all other cases.
        if (
            self._env is not None
            and threading.current_thread() is threading.main_thread()
        ):
            self._env.close()
            self._env = None


class VecPettingZooToGymServiceServicer(GymServiceServicer):
    """
    gRPC servicer that wraps multiple PettingZoo Parallel environments.
    Provides vectorized execution of multiple independent PettingZoo environments.
    """

    def __init__(self, env_factories: List[Callable], wrappers: Optional[list] = None):
        """
        Initialize the vectorized PettingZoo servicer.

        Args:
            env_factories: List of callables that return PettingZoo environments
            wrappers: Optional list of wrapper classes to apply to each environment
        """
        self._env_factories = [
            functools.partial(wrap, env_func, wrappers) for env_func in env_factories
        ]
        self._n_envs = len(self._env_factories)

        self._last_reset_obs = None
        self._last_reset_info = None
        self._envs = None
        self._wrapper_classes = wrappers if wrappers else []
        self._autoreset_envs = np.array(
            [False for _ in range(self._n_envs)], dtype=bool
        )

    @property
    def envs(self):
        assert self._envs is not None, "Environments not initialized"
        return self._envs

    @capture_traceback
    def UpdateState(self, request: StateUpdate, context) -> State:
        """Handle reset and step requests for all environments."""

        msg_type = request.WhichOneof("update")

        if msg_type == "reset":
            initial_state = InitialState()

            for env_idx in range(self._n_envs):
                seed = (
                    request.reset.environments[env_idx].seed
                    if request.reset.environments[env_idx].HasField("seed")
                    else None
                )
                options = (
                    dict(request.reset.environments[env_idx].options)
                    if request.reset.environments[env_idx].options
                    else None
                )

                # Reset the environment
                observations, infos = self.envs[env_idx].reset(
                    seed=seed, options=options
                )

                # Convert to protobuf format
                agent_states = {}
                for agent_id, obs in observations.items():
                    obs_space = self.envs[env_idx].observation_space(agent_id)
                    agent_states[agent_id] = InitialAgentState(
                        observations=make_generic(to_proto(obs_space, obs)),
                        info=infos.get(agent_id, {}),
                    )

                initial_state.environment_states[env_idx].CopyFrom(
                    InitialEnvironmentState(agent_states=agent_states)
                )

            return State(initial_state=initial_state)

        elif msg_type == "step":
            # Extract actions for each environment
            actions = [{} for _ in range(self._n_envs)]
            for env_idx in range(self._n_envs):
                for agent_id, action_proto in request.step.environments[
                    env_idx
                ].updates.items():
                    actions[env_idx][agent_id] = from_proto(action_proto)

            # Collect results
            observations_list = [{} for _ in range(self._n_envs)]
            rewards_list = [{} for _ in range(self._n_envs)]
            terminations_list = [{} for _ in range(self._n_envs)]
            truncations_list = [{} for _ in range(self._n_envs)]
            infos_list = [{} for _ in range(self._n_envs)]

            initial_obs_dict = {}
            initial_infos_dict = {}

            # Step each environment
            for env_idx, actions in enumerate(actions):
                # Handle autoreset
                if self.autoreset_mode == AutoResetType.NEXT_STEP:
                    if self._autoreset_envs[env_idx]:
                        # Reset this environment
                        obs, info = self.envs[env_idx].reset()
                        # Create dummy step results for reset
                        rewards_list[env_idx] = {
                            agent_id: 0.0
                            for agent_id in self.envs[env_idx].possible_agents
                        }
                        terminations_list[env_idx] = {
                            agent_id: False
                            for agent_id in self.envs[env_idx].possible_agents
                        }
                        truncations_list[env_idx] = {
                            agent_id: False
                            for agent_id in self.envs[env_idx].possible_agents
                        }
                        observations_list[env_idx] = obs

                    else:
                        # Normal step
                        assert (
                            actions != {}
                        ), f"Got an empty action for environment {env_idx}, indicating we should be auto-resetting but aren't"
                        obs, rewards, terminations, truncations, infos = self.envs[
                            env_idx
                        ].step(actions)
                        observations_list[env_idx] = obs
                        rewards_list[env_idx] = rewards
                        terminations_list[env_idx] = terminations
                        truncations_list[env_idx] = truncations
                        infos_list[env_idx] = infos

                elif self.autoreset_mode == AutoResetType.DISABLED:
                    # No autoreset - assumes user handles it
                    assert not self._autoreset_envs[
                        env_idx
                    ], f"Environment {env_idx} needs reset but autoreset is disabled"
                    obs, rewards, terminations, truncations, infos = self.envs[
                        env_idx
                    ].step(actions)
                    observations_list[env_idx] = obs
                    rewards_list[env_idx] = rewards
                    terminations_list[env_idx] = terminations
                    truncations_list[env_idx] = truncations
                    infos_list[env_idx] = infos

                elif self.autoreset_mode == AutoResetType.SAME_STEP:
                    # Step and immediately reset if done
                    obs, rewards, terminations, truncations, infos = self.envs[
                        env_idx
                    ].step(actions)
                    observations_list[env_idx] = obs
                    rewards_list[env_idx] = rewards
                    terminations_list[env_idx] = terminations
                    truncations_list[env_idx] = truncations
                    infos_list[env_idx] = infos

                    # Check if all agents are done
                    all_done = all(terminations.values()) or all(truncations.values())
                    if all_done:
                        initial_obs, initial_info = self.envs[env_idx].reset()
                        initial_obs_dict[env_idx] = initial_obs
                        initial_infos_dict[env_idx] = initial_info

            # Update autoreset tracking
            self._autoreset_envs = np.array(
                [
                    all(terminations_list[i].values())
                    or all(truncations_list[i].values())
                    for i in range(self._n_envs)
                ],
                dtype=bool,
            )

            # Build output state
            output_state = State()
            training_state = output_state.training_state

            for env_idx in range(self._n_envs):
                agent_states = {}
                for agent_id, obs in observations_list[env_idx].items():
                    obs_space = self.envs[env_idx].observation_space(agent_id)
                    agent_states[agent_id] = AgentState(
                        observations=make_generic(to_proto(obs_space, obs)),
                        reward=float(rewards_list[env_idx].get(agent_id, 0.0)),
                        terminated=bool(
                            terminations_list[env_idx].get(agent_id, False)
                        ),
                        truncated=bool(truncations_list[env_idx].get(agent_id, False)),
                        info=infos_list[env_idx].get(agent_id, {}),
                    )

                training_state.environment_states.add().CopyFrom(
                    EnvironmentState(agent_states=agent_states)
                )

                # Add initial observations for same-step autoreset
                if env_idx in initial_obs_dict:
                    initial_agent_states = {}
                    for agent_id, obs in initial_obs_dict[env_idx].items():
                        obs_space = self.envs[env_idx].observation_space(agent_id)
                        initial_agent_states[agent_id] = InitialAgentState(
                            observations=make_generic(to_proto(obs_space, obs)),
                            info=initial_infos_dict[env_idx].get(agent_id, {}),
                        )
                    output_state.initial_state.environment_states[env_idx].CopyFrom(
                        InitialEnvironmentState(agent_states=initial_agent_states)
                    )

            return output_state

        elif request.status == CLOSED:
            return State()
        elif request.status == ERROR:
            return State()
        else:
            raise ValueError("Invalid update message")

    def RequestInitialTrainingState(
        self, request: InitialStateRequest, context
    ) -> InitialState:
        """Request the initial state (empty for vectorized)."""
        return InitialState()

    @capture_traceback
    def RequestTrainingDefinition(
        self, request: TrainingDefinitionRequest, context
    ) -> TrainingDefinition:
        """Define the observation and action spaces for all environments."""
        env_defn_list = []

        for env_idx in range(self._n_envs):
            agent_definitions = {}

            # Get spaces for each agent in this environment
            for agent_id in self.envs[env_idx].possible_agents:
                obs_space = self.envs[env_idx].observation_space(agent_id)
                action_space = self.envs[env_idx].action_space(agent_id)

                agent_definitions[agent_id] = AgentDefinition(
                    obs_space=make_generic(space_to_proto(obs_space)),
                    action_space=make_generic(space_to_proto(action_space)),
                )

            env_defn_list.append(
                EnvironmentDefinition(agent_definitions=agent_definitions)
            )

        return TrainingDefinition(environment_definitions=env_defn_list)

    @capture_traceback
    def StartGymConnector(
        self, request: GymConnectorStartRequest, context
    ) -> GymConnectorStartResponse:
        """Initialize all environments."""
        self._envs = [factory() for factory in self._env_factories]
        self.autoreset_mode = request.autoreset_type
        return GymConnectorStartResponse()

    def close(self):
        """Explicitly close all environments. Must be called from the main thread for pygame-backed envs."""
        if self._envs is not None:
            for env in self._envs:
                env.close()
            self._envs = None

    def __del__(self):
        # pygame's SDL is not thread-safe; calling display teardown from a background
        # thread (e.g. gRPC ThreadPoolExecutor or GC) causes a segfault. Only close
        # from the main thread; explicit close() handles cleanup in all other cases.
        if (
            self._envs is not None
            and threading.current_thread() is threading.main_thread()
        ):
            for env in self._envs:
                env.close()
            self._envs = None
