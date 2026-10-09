# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Vector multi-agent RLlib tests for Schola environments."""

import pytest
from ray.rllib.core.rl_module.default_model_config import DefaultModelConfig
from ray.rllib.core.rl_module.multi_rl_module import MultiRLModuleSpec
from ray.rllib.core.rl_module.rl_module import RLModuleSpec
from ray.rllib.env import EnvContext
from schola.rllib.env_runner import ScholaEnvRunner
from schola.rllib.env import RayVecEnv
from ray.rllib.env.multi_agent_env import MultiAgentEnv
from ray.rllib.env.vector.vector_multi_agent_env import VectorMultiAgentEnv
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
import gymnasium as gym
import numpy as np

# Check if PettingZoo is available
try:
    import pettingzoo

    PETTINGZOO_AVAILABLE = True
except ImportError:
    PETTINGZOO_AVAILABLE = False

# Skip all tests in this file if PettingZoo is not available
pytestmark = pytest.mark.skipif(
    not PETTINGZOO_AVAILABLE, reason="PettingZoo not installed"
)


def test_rayvecenv_pettingzoo_create(
    make_rllib_vec_pettingzoo_env, make_pettingzoo_env
):
    """Test creating RayVecEnv with PettingZoo environments."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    assert env.num_envs == 2, f"Expected 2 environments, got {env.num_envs}"
    assert (
        env.single_observation_space is not None
    ), f"Single observation space should not be None"
    assert (
        env.single_action_space is not None
    ), f"Single action space should not be None"
    assert (
        env.single_observation_spaces is not None
    ), f"Single observation spaces should not be None"
    assert (
        env.single_action_spaces is not None
    ), f"Single action spaces should not be None"

    # PettingZoo environments have multiple agents
    assert (
        len(env.possible_agents) == 3
    ), "PettingZoo simple_spread_v3 should have multiple agents"

    # Check that all agents have defined spaces
    for agent_id in env.possible_agents:
        assert (
            agent_id in env.single_observation_spaces
        ), f"Agent {agent_id} is not in single observation spaces"
        assert (
            agent_id in env.single_action_spaces
        ), f"Agent {agent_id} is not in single action spaces"

    # from PettingZoo documentation:
    target_obs_space = gym.spaces.Box(low=-np.inf, high=np.inf, shape=(18,))
    target_action_space = gym.spaces.Discrete(5)

    for agent_id in env.possible_agents:
        obs_space = env.single_observation_spaces[agent_id]
        action_space = env.single_action_spaces[agent_id]
        assert (
            obs_space == target_obs_space
        ), f"Observation space for agent {agent_id} is not correct. Got {obs_space} Expected {target_obs_space}"
        assert (
            action_space == target_action_space
        ), f"Action space for agent {agent_id} is not correct. Got {action_space} Expected {target_action_space}"

    env.close()


def test_rayvecenv_pettingzoo_reset(make_rllib_vec_pettingzoo_env, make_pettingzoo_env):
    """Test RayVecEnv reset with PettingZoo environments."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    observations, infos = env.reset()

    # Should return list format
    assert isinstance(observations, list)
    assert isinstance(infos, list)
    assert len(observations) == 2
    assert len(infos) == 2

    # Each observation should be a dict with multiple agents
    for obs in observations:
        assert isinstance(obs, dict)
        assert len(obs) == 3, "Each environment should have 3 agents"

        # All agents should be in possible_agents
        for agent_id in obs.keys():
            assert agent_id in env.possible_agents

    env.close()


def test_rayvecenv_pettingzoo_step(make_rllib_vec_pettingzoo_env, make_pettingzoo_env):
    """Test RayVecEnv step with PettingZoo environments."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    observations, infos = env.reset()

    # Create actions for all agents in all environments
    actions = []
    for obs in observations:
        action = {}
        for agent_id in obs.keys():
            action_space = env.single_action_spaces[agent_id]
            action[agent_id] = action_space.sample()
        actions.append(action)

    observations, rewards, terminateds, truncateds, infos = env.step(actions)

    # Check return types
    assert isinstance(observations, list)
    assert isinstance(rewards, list)
    assert isinstance(terminateds, list)
    assert isinstance(truncateds, list)
    assert isinstance(infos, list)
    assert len(observations) == 2

    # Check structure of each environment
    for i in range(2):
        assert isinstance(observations[i], dict)
        assert isinstance(rewards[i], dict)
        assert isinstance(terminateds[i], dict)
        assert isinstance(truncateds[i], dict)
        assert isinstance(infos[i], dict)

        # Check for __all__ key
        assert "__all__" in terminateds[i]
        assert "__all__" in truncateds[i]

    env.close()


def test_rayvecenv_pettingzoo_multiple_steps(
    make_rllib_vec_pettingzoo_env, make_pettingzoo_env
):
    """Test RayVecEnv with multiple steps and multiple PettingZoo environments."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(3)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    observations, infos = env.reset()

    # Run for several steps
    for step in range(20):
        # Create actions
        actions = []
        for obs in observations:
            action = {}
            for agent_id in obs.keys():
                action_space = env.single_action_spaces[agent_id]
                action[agent_id] = action_space.sample()
            actions.append(action)

        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        # Verify structure is maintained
        assert len(observations) == 3
        assert len(rewards) == 3

        # Each environment should have all its agents
        for env_idx in range(3):
            for agent_id in observations[env_idx].keys():
                if agent_id != "__all__":
                    assert agent_id in env.possible_agents

    env.close()


def test_rayvecenv_pettingzoo_agent_tracking(
    make_rllib_vec_pettingzoo_env, make_pettingzoo_env
):
    """Test that agent tracking works correctly across wrappers."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    observations, infos = env.reset()

    # Check that envs list has the correct structure
    assert hasattr(env, "envs")
    assert len(env.envs) == 2

    # Each wrapper should be a MultiAgentEnv
    for wrapper in env.envs:
        assert isinstance(wrapper, MultiAgentEnv)
        assert hasattr(wrapper, "agents")
        assert hasattr(wrapper, "possible_agents")

        # After reset, agents should be populated
        assert len(wrapper.agents) == 3

    env.close()


def test_rayvecenv_pettingzoo_inheritance(
    make_rllib_vec_pettingzoo_env, make_pettingzoo_env
):
    """Test that RayVecEnv with PettingZoo inherits from correct base classes."""
    env_fns = [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    env = make_rllib_vec_pettingzoo_env(env_fns)

    # Check inheritance
    assert isinstance(
        env, VectorMultiAgentEnv
    ), "Should inherit from VectorMultiAgentEnv"

    # Check that required properties exist
    assert hasattr(env, "possible_agents")
    assert hasattr(env, "num_envs")
    assert hasattr(env, "observation_space")
    assert hasattr(env, "action_space")

    env.close()


from ray.rllib.algorithms.ppo import PPOConfig
from ray.rllib.policy.policy import PolicySpec
from ray.rllib.connectors.env_to_module import FlattenObservations


@pytest.mark.xdist_group(name="ray-cluster")
def test_rayvecenv_multiagent_training(
    make_vec_pettingzoo_env_server, make_pettingzoo_env, ray_cluster
):
    env_server_port = make_vec_pettingzoo_env_server(
        [make_pettingzoo_env("simple_spread_v3") for _ in range(2)]
    )

    config = (
        PPOConfig()
        .api_stack(
            enable_rl_module_and_learner=True,
            enable_env_runner_and_connector_v2=True,
        )
        .training(
            num_epochs=2,
            train_batch_size=128,
            minibatch_size=32,
        )
        .environment(
            env_config={
                "protocol": GrpcProtocol,
                "protocol_args": {"url": "localhost", "port": env_server_port},
                "simulator": UnrealEditor,
                "simulator_args": {},
            },
        )
        .framework("torch")
        .env_runners(
            env_runner_cls=ScholaEnvRunner,
            num_env_runners=0,  # Use local runner only (no remote workers)
            env_to_module_connector=lambda env, spaces=None, device=None: FlattenObservations(
                input_observation_space=env.single_observation_space,
                input_action_space=env.single_action_space,
                multi_agent=True,
            ),
        )
        .multi_agent(
            policies={"shared_policy": PolicySpec()},
            policy_mapping_fn=lambda agent_id, *args, **kwargs: "shared_policy",
        )
        .rl_module(
            rl_module_spec=MultiRLModuleSpec(
                rl_module_specs={
                    "shared_policy": RLModuleSpec(
                        model_config={
                            "fcnet_hiddens": [32, 32],
                            "vf_share_layers": True,
                        }
                    )
                }
            ),
        )
    )

    algo = config.build_algo()
    # Just run a few iterations to verify training works
    result = algo.train()
    assert result is not None
    algo.stop()
