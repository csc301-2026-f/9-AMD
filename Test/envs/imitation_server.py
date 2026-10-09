# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""gRPC servicers for imitation / offline connector flows used in tests."""

import functools
from typing import Callable, List
import numpy as np
from schola.generated.Definitions_pb2 import (
    AgentDefinition,
    EnvironmentDefinition,
    TrainingDefinition,
)
from schola.generated.ImitationConnector_pb2 import (
    ImitationConnectorStartRequest,
    ImitationConnectorStartResponse,
    ImitationDefinitionRequest,
    ImitationStateRequest,
)
from schola.generated.ImitationState_pb2 import (
    ImitationAgentState,
    ImitationEnvironmentState,
    ImitationState,
    ImitationTrainingState,
)
from schola.generated.State_pb2 import (
    InitialAgentState,
    InitialEnvironmentState,
    InitialState,
)
from schola.core.protocols.protobuf.deserialize import from_proto
from schola.core.protocols.protobuf.serialize import (
    space_to_proto,
    to_proto,
    make_generic,
    fill_generic,
)
from schola.generated.ImitationConnector_pb2_grpc import (
    ImitationConnectorServiceServicer,
)
from schola.generated.ImitationConnector_pb2 import *
import grpc
import gymnasium as gym
from schola.generated.ImitationState_pb2 import *


def wrap(env, wrappers):
    if not isinstance(env, gym.Env):
        env = env()
    if wrappers:
        for wrapper in wrappers:
            env = wrapper(env)
    return env


class GymToImitationServiceServicer(ImitationConnectorServiceServicer):

    def __init__(
        self,
        env_id: str | Callable[..., gym.Env],
        policy_class,
        wrappers: list = None,
    ):
        if isinstance(env_id, str):
            self._env_factory = lambda: gym.make(env_id)
        else:
            self._env_factory = env_id
        self._agent_id = "single_agent"
        self._env = None
        self._wrapper_classes = wrappers if wrappers else []
        self._last_obs = None
        self._policy_class = policy_class
        self._policy = None
        self._first_step = True
        self._seed = None
        self._options = None

    def _make_initial_state(self, obs, info=None):
        init_agent_state = InitialAgentState(
            observations=make_generic(to_proto(self._env.observation_space, obs)),
            info=info if info else {},
        )
        init_state = InitialState(
            environment_states={
                0: InitialEnvironmentState(
                    agent_states={self._agent_id: init_agent_state}
                )
            }
        )
        return init_state

    def RequestState(self, request: ImitationStateRequest, context) -> ImitationState:
        # Get action from policy using current observation

        init_state = None
        # reset on the first step and get the initial state
        if self._first_step:
            initial_obs, initial_info = self._env.reset(
                seed=self._seed, options=self._options
            )
            init_state = self._make_initial_state(initial_obs, initial_info)
            self._last_obs = initial_obs
            self._first_step = False

        action = self._policy(self._last_obs)

        obs, reward, terminated, truncated, info = self._env.step(action)

        # self reset if terminated or truncated
        if terminated or truncated:
            initial_obs, initial_info = self._env.reset()
            init_state = self._make_initial_state(initial_obs, initial_info)
            self._last_obs = initial_obs
        else:
            self._last_obs = obs

        output = ImitationState(
            training_state=ImitationTrainingState(
                environment_states=[
                    ImitationEnvironmentState(
                        agent_states={
                            self._agent_id: ImitationAgentState(
                                observations=make_generic(
                                    to_proto(self._env.observation_space, obs)
                                ),
                                reward=reward,
                                terminated=terminated,
                                truncated=truncated,
                                info=info if info else {},
                                actions=make_generic(
                                    to_proto(self._env.action_space, action)
                                ),
                            )
                        }
                    )
                ]
            ),
            initial_state=init_state,
        )

        return output

    def RequestTrainingDefinition(
        self, request: ImitationDefinitionRequest, context
    ) -> TrainingDefinition:
        self._env = wrap(self._env_factory, self._wrapper_classes)
        self._policy = self._policy_class(self._env)
        return TrainingDefinition(
            environment_definitions=[
                EnvironmentDefinition(
                    agent_definitions={
                        self._agent_id: AgentDefinition(
                            obs_space=make_generic(
                                space_to_proto(self._env.observation_space)
                            ),
                            action_space=make_generic(
                                space_to_proto(self._env.action_space)
                            ),
                        )
                    }
                )
            ]
        )

    def StartImitationConnector(
        self, request: ImitationConnectorStartRequest, context
    ) -> ImitationConnectorStartResponse:
        if request.environments.get(0) is not None:
            self._seed = request.environments[0].seed
            self._options = request.environments[0].options

        return ImitationConnectorStartResponse()

    def __del__(self):
        if self._env is not None:
            self._env.close()
        self._env = None
