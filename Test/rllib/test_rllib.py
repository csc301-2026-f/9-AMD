# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""End-to-end tests for ``RayEnv`` with RLlib algorithms."""

import pytest
import numpy as np
import gymnasium as gym
from ray import tune
from ray.rllib.algorithms.ppo import PPOConfig
from ray.rllib.connectors.env_to_module import FlattenObservations
from ray.rllib.core.rl_module.default_model_config import DefaultModelConfig
from ray.rllib.core.rl_module.multi_rl_module import MultiRLModuleSpec
from ray.rllib.core.rl_module.rl_module import RLModuleSpec
from ray.rllib.policy.policy import PolicySpec
from ray.tune.registry import register_env
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from schola.rllib.env import RayEnv, BaseRayEnv
from ray.rllib.env.multi_agent_env import MultiAgentEnv


def test_rayenv_create(make_rllib_env):
    """Test creating the RayEnv single environment."""
    env = make_rllib_env("CartPole-v1")

    assert env.num_envs == 1
    assert env.single_observation_space is not None
    assert env.single_action_space is not None
    assert env.single_observation_spaces is not None
    assert env.single_action_spaces is not None

    env.close()


def test_rayenv_reset(make_rllib_env):
    """Test RayEnv reset function returns dict format."""
    env = make_rllib_env("CartPole-v1")

    observations, infos = env.reset()

    # RayEnv returns dict format (not list)
    assert isinstance(observations, dict)
    assert isinstance(infos, dict)
    assert len(observations) > 0
    assert len(infos) > 0

    # Check that observation contains agent_ids
    for agent_id in observations.keys():
        assert agent_id in env.single_observation_spaces
        obs_value = observations[agent_id]
        assert obs_value in env.single_observation_spaces[agent_id]

    env.close()


def test_rayenv_reset_with_seed(make_rllib_env):
    """Test RayEnv reset with seed produces reproducible results."""
    env = make_rllib_env("CartPole-v1")

    observations1, infos1 = env.reset(seed=42)
    observations2, infos2 = env.reset(seed=42)

    # With the same seed, observations should be reproducible
    assert observations1.keys() == observations2.keys()
    for agent_id in observations1.keys():
        obs1 = observations1[agent_id]
        obs2 = observations2[agent_id]
        assert np.allclose(obs1, obs2), f"Observations differ for agent {agent_id}"

    env.close()


def test_rayenv_step(make_rllib_env):
    """Test RayEnv step function returns dict format."""
    # Use CartPole instead of FrozenLake to avoid gRPC issues
    env = make_rllib_env("CartPole-v1")

    observations, infos = env.reset()

    # Create actions dict
    actions = {}
    for agent_id in observations.keys():
        action_space = env.single_action_spaces[agent_id]
        actions[agent_id] = (
            int(action_space.sample())
            if hasattr(action_space.sample(), "item")
            else action_space.sample()
        )

    observations, rewards, terminateds, truncateds, infos = env.step(actions)

    # RayEnv returns dict format (not list)
    assert isinstance(observations, dict)
    assert isinstance(rewards, dict)
    assert isinstance(terminateds, dict)
    assert isinstance(truncateds, dict)
    assert isinstance(infos, dict)

    # Check for __all__ keys in terminated/truncated
    assert "__all__" in terminateds
    assert "__all__" in truncateds

    # Check observations have correct structure
    for agent_id in observations.keys():
        if agent_id != "__all__":
            obs = observations[agent_id]
            assert obs in env.single_observation_spaces[agent_id]

    # Check types
    for key in rewards.keys():
        assert isinstance(rewards[key], (int, float, np.integer, np.floating))

    for key in terminateds.keys():
        assert isinstance(terminateds[key], (bool, np.bool_))

    for key in truncateds.keys():
        assert isinstance(truncateds[key], (bool, np.bool_))

    env.close()


# TODO this test doesn't check the correct thing, it should check that an exception is raised if multiple environments are detected
def test_rayenv_single_env_constraint(make_rllib_env):
    """Test that RayEnv validates single environment constraint."""
    # This test verifies that RayEnv raises an error if multiple environments are detected
    # The constraint is checked in _define_environment
    env = make_rllib_env("CartPole-v1")

    # RayEnv should have exactly 1 environment
    assert env.num_envs == 1

    env.close()


def test_rayenv_spaces(make_rllib_env):
    """Test RayEnv observation and action space properties."""
    env = make_rllib_env("CartPole-v1")

    # Check that spaces are properly defined
    assert env.observation_space is not None
    assert env.action_space is not None
    assert env.single_observation_space is not None
    assert env.single_action_space is not None

    # Check that single spaces are Dict spaces
    assert isinstance(env.single_observation_space, gym.spaces.Dict)
    assert isinstance(env.single_action_space, gym.spaces.Dict)

    # Check that single_observation_spaces and single_action_spaces are dicts
    assert isinstance(env.single_observation_spaces, dict)
    assert isinstance(env.single_action_spaces, dict)
    assert len(env.single_observation_spaces) > 0
    assert len(env.single_action_spaces) > 0

    env.close()


def test_rayenv_agents_property(make_rllib_env):
    """Test RayEnv agents property."""
    env: RayEnv = make_rllib_env("CartPole-v1")

    observations, infos = env.reset()

    # Agents property should return a list (RayEnv updates self.agents as list)
    agents = env.agents
    assert isinstance(
        agents, list
    ), f"agents property should return a list, got {type(agents)}"
    assert (
        len(agents) > 0
    ), "agents property should contain at least one agent after reset"

    # All agents in observations should be in agents property
    for agent_id in observations.keys():
        assert (
            agent_id in agents
        ), f"Agent {agent_id} in observations should be in agents property"

    # Agents should be a subset of possible_agents
    assert set(agents).issubset(
        env.possible_agents
    ), "agents should be a subset of possible_agents"

    env.close()


def test_baserayenv_inheritance_rayenv(make_rllib_env):
    """Test that RayEnv inherits from BaseRayEnv."""
    env = make_rllib_env("CartPole-v1")

    # Check inheritance
    assert isinstance(env, BaseRayEnv), "RayEnv should inherit from BaseRayEnv"
    assert isinstance(env, MultiAgentEnv), "RayEnv should inherit from MultiAgentEnv"

    # Check that BaseRayEnv properties are accessible
    assert hasattr(env, "protocol")
    assert hasattr(env, "simulator")
    assert hasattr(env, "possible_agents")
    assert hasattr(env, "num_agents")
    assert hasattr(env, "max_num_agents")
    assert hasattr(env, "observation_space")
    assert hasattr(env, "action_space")
    assert hasattr(env, "single_observation_space")
    assert hasattr(env, "single_action_space")
    assert hasattr(env, "single_observation_spaces")
    assert hasattr(env, "single_action_spaces")

    env.close()


def test_baserayenv_possible_agents_static(make_rllib_env):
    """Test that possible_agents is static (doesn't change after initialization)."""
    env = make_rllib_env("CartPole-v1")

    initial_possible = env.possible_agents.copy()

    # Reset multiple times
    for _ in range(5):
        env.reset()
        current_possible = env.possible_agents.copy()
        assert (
            initial_possible == current_possible
        ), "possible_agents should remain static after initialization"

    # Step multiple times
    observations, _ = env.reset()
    actions = {
        agent_id: env.single_action_spaces[agent_id].sample()
        for agent_id in observations.keys()
    }

    for _ in range(10):
        obs, rewards, terminateds, truncateds, infos = env.step(actions)
        if terminateds.get("__all__", False) or truncateds.get("__all__", False):
            obs, _ = env.reset()
            actions = {
                agent_id: env.single_action_spaces[agent_id].sample()
                for agent_id in obs.keys()
            }
        current_possible = env.possible_agents.copy()
        assert (
            initial_possible == current_possible
        ), "possible_agents should remain static during steps"

    env.close()


def test_baserayenv_close_extras(make_rllib_env):
    """Test that BaseRayEnv close_extras works for RayEnv."""
    env = make_rllib_env("CartPole-v1")
    assert hasattr(env, "close_extras")
    env.close_extras()


def test_baserayenv_shared_properties(make_rllib_env, make_rllib_vec_env, make_env):
    """Test that BaseRayEnv properties work consistently across both classes."""
    # Test RayEnv
    ray_env = make_rllib_env("CartPole-v1")
    ray_env.reset()

    # Test RayVecEnv
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    ray_vec_env = make_rllib_vec_env(env_fns)
    ray_vec_env.reset()

    # Both should have the same property structure from BaseRayEnv
    shared_properties = [
        "num_agents",
        "max_num_agents",
        "observation_space",
        "action_space",
        "single_observation_space",
        "single_action_space",
        "single_observation_spaces",
        "single_action_spaces",
    ]

    for prop in shared_properties:
        assert hasattr(ray_env, prop), f"RayEnv should have {prop} property"
        assert hasattr(ray_vec_env, prop), f"RayVecEnv should have {prop} property"

        # Values should be comparable (same environment type)
        ray_env_val = getattr(ray_env, prop)
        ray_vec_env_val = getattr(ray_vec_env, prop)

        # Both should have non-None values
        assert ray_env_val is not None, f"RayEnv.{prop} should not be None"
        assert ray_vec_env_val is not None, f"RayVecEnv.{prop} should not be None"

    ray_env.close()
    ray_vec_env.close()


def test_rayenv_vs_rayvecenv_return_formats(
    make_rllib_env, make_rllib_vec_env, make_env
):
    """Test that RayEnv returns dicts and RayVecEnv returns lists."""

    # Test RayEnv returns dict
    ray_env = make_rllib_env("CartPole-v1")
    obs, info = ray_env.reset()
    assert isinstance(obs, dict), "RayEnv.reset() should return dict"
    assert isinstance(info, dict), "RayEnv.reset() should return dict"

    actions = {
        agent_id: ray_env.single_action_spaces[agent_id].sample()
        for agent_id in obs.keys()
    }
    obs, rewards, terminateds, truncateds, info = ray_env.step(actions)
    assert isinstance(obs, dict), "RayEnv.step() should return dict"
    assert isinstance(rewards, dict), "RayEnv.step() should return dict"
    assert isinstance(terminateds, dict), "RayEnv.step() should return dict"
    assert isinstance(truncateds, dict), "RayEnv.step() should return dict"
    assert isinstance(info, dict), "RayEnv.step() should return dict"

    ray_env.close()

    # Test RayVecEnv returns list
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    ray_vec_env = make_rllib_vec_env(env_fns)
    obs_list, info_list = ray_vec_env.reset()
    assert isinstance(obs_list, list), "RayVecEnv.reset() should return list"
    assert isinstance(info_list, list), "RayVecEnv.reset() should return list"
    assert len(obs_list) == 2
    assert len(info_list) == 2

    actions_list = [
        {
            agent_id: ray_vec_env.single_action_spaces[agent_id].sample()
            for agent_id in obs.keys()
        }
        for obs in obs_list
    ]
    obs_list, rewards_list, terminateds_list, truncateds_list, info_list = (
        ray_vec_env.step(actions_list)
    )
    assert isinstance(obs_list, list), "RayVecEnv.step() should return list"
    assert isinstance(rewards_list, list), "RayVecEnv.step() should return list"
    assert isinstance(terminateds_list, list), "RayVecEnv.step() should return list"
    assert isinstance(truncateds_list, list), "RayVecEnv.step() should return list"
    assert isinstance(info_list, list), "RayVecEnv.step() should return list"

    ray_vec_env.close()


@pytest.mark.xdist_group(name="ray-cluster")
def test_training(make_env_server, ray_cluster):
    env_server_port = make_env_server("CartPole-v1")

    def make(*args, **kwargs):
        simulator = UnrealEditor()
        protocol = GrpcProtocol(url="localhost", port=env_server_port)
        return RayEnv(protocol, simulator)

    register_env("ScholaCartPole-v1", make)

    config = (
        PPOConfig()
        .training(
            num_epochs=2,
            train_batch_size=128,
            minibatch_size=32,
        )
        .api_stack(
            enable_rl_module_and_learner=True,
            enable_env_runner_and_connector_v2=True,
        )
        .environment(env="ScholaCartPole-v1")
        .framework("torch")
        .env_runners(
            num_env_runners=0,  # Use local runner only (no remote workers)
            env_to_module_connector=lambda env, spaces=None, device=None: FlattenObservations(
                input_observation_space=env.single_observation_space,
                input_action_space=env.single_action_space,
                multi_agent=True,
            ),
        )
        .learners(
            num_learners=0,
        )
        .multi_agent(
            policies={"shared_policy": PolicySpec()},
            policy_mapping_fn=lambda agent_id, *args, **kwargs: "shared_policy",
        )
        .rl_module(
            rl_module_spec=MultiRLModuleSpec(
                rl_module_specs={
                    "shared_policy": RLModuleSpec(
                        model_config=DefaultModelConfig(
                            fcnet_hiddens=[32, 32], vf_share_layers=True
                        )
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
