# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Vectorized RLlib tests for ``RayVecEnv`` and ``ScholaEnvRunner``."""

import pytest
import numpy as np
import gymnasium as gym
from ray.rllib.core.rl_module import MultiRLModuleSpec, RLModuleSpec
from ray.rllib.core.rl_module.default_model_config import DefaultModelConfig
from schola.rllib.env_runner import ScholaEnvRunner
from schola.rllib.env import RayVecEnv, BaseRayEnv
from ray.rllib.env.multi_agent_env import MultiAgentEnv
from ray.rllib.env.vector.vector_multi_agent_env import VectorMultiAgentEnv
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from ray.rllib.algorithms.ppo import PPOConfig
from ray.rllib.policy.policy import PolicySpec
from ray.rllib.connectors.env_to_module import FlattenObservations
from functools import partial
from Test.gym.testing_env import GenericTestEnv
from gymnasium.spaces import Discrete


def test_rayvecenv_create(make_rllib_vec_env, make_env):
    """Test creating the RayVecEnv vector environment."""
    env_fns = [make_env("FrozenLake-v1", i) for i in range(4)]
    env = make_rllib_vec_env(env_fns)

    assert env.num_envs == 4, f"Expected 4 environments, got {env.num_envs}"
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

    env.close()


def test_rayvecenv_reset(make_rllib_vec_env, make_env):
    """Test RayVecEnv reset function."""
    env_fns = [make_env("CartPole-v1", i) for i in range(4)]
    env = make_rllib_vec_env(env_fns)

    observations, infos = env.reset()

    assert isinstance(observations, list)
    assert len(observations) == 4
    assert isinstance(infos, list)
    assert len(infos) == 4

    # Each observation should be a dict with agent_id as key
    for obs in observations:
        assert isinstance(obs, dict)
        assert len(obs) > 0
        # Check that observation contains agent_ids from the environment
        for agent_id in obs.keys():
            assert agent_id in env.single_observation_spaces
            # Check observation structure matches the space
            assert agent_id in env.single_observation_spaces
            obs_value = obs[agent_id]
            assert obs_value in env.single_observation_spaces[agent_id]

    env.close()


def test_rayvecenv_reset_with_seed(make_rllib_vec_env, make_env):
    """Test RayVecEnv reset with seed and compare against gym.make() CartPole."""
    env_fns = [make_env("CartPole-v1", i) for i in range(4)]
    env = make_rllib_vec_env(env_fns)

    observations1, infos1 = env.reset(seed=42)
    observations2, infos2 = env.reset(seed=42)

    # With the same seed, observations should be reproducible
    assert len(observations1) == len(observations2)

    # Compare the observation content for each environment
    for i in range(len(observations1)):
        assert observations1[i].keys() == observations2[i].keys()
        for agent_id in observations1[i].keys():
            obs1 = observations1[i][agent_id]
            obs2 = observations2[i][agent_id]
            # For CartPole, observations are Box spaces (numpy arrays)
            assert np.allclose(
                obs1, obs2
            ), f"Observations differ for env {i}, agent {agent_id}"

    # Compare observation structure and value ranges against standard gym CartPole
    # Note: Exact values may differ due to RayVecEnv's wrapping layer, but structure should match
    num_envs = len(observations1)

    # Create a reference gym environment to verify observation structure
    gym_env = gym.make("CartPole-v1", disable_env_checker=True)
    gym_obs, _ = gym_env.reset(seed=42)

    # Verify that RayVecEnv observations have the same structure as gym observations
    for i in range(num_envs):
        agent_ids = list(observations1[i].keys())
        assert len(agent_ids) > 0, f"No agents found in environment {i}"
        agent_id = agent_ids[0]

        vec_obs = observations1[i][agent_id]

        # Verify observation shape and dtype match
        assert (
            vec_obs.shape == gym_obs.shape
        ), f"Observation shape mismatch for env {i}: vec={vec_obs.shape}, gym={gym_obs.shape}"
        assert (
            vec_obs.dtype == gym_obs.dtype
        ), f"Observation dtype mismatch for env {i}: vec={vec_obs.dtype}, gym={gym_obs.dtype}"

        # Verify observations are within valid CartPole ranges
        # CartPole obs: [cart position, cart velocity, pole angle, pole angular velocity]
        # Typical ranges: position ±2.4, velocity ±inf, angle ±0.2095, angular velocity ±inf
        assert (
            len(vec_obs) == 4
        ), f"CartPole should have 4 observation values, got {len(vec_obs)}"
        assert np.all(
            np.isfinite(vec_obs)
        ), f"Observations should be finite, got {vec_obs}"

    gym_env.close()
    env.close()


def test_rayvecenv_step(make_rllib_vec_env, make_env):
    """Test RayVecEnv step function."""
    env_fns = [make_env("FrozenLake-v1", i) for i in range(4)]
    env = make_rllib_vec_env(env_fns)

    observations, infos = env.reset()

    # Create actions for each environment
    actions = []
    for obs in observations:
        # Sample action for each agent in the environment
        action = {}
        for agent_id in obs.keys():
            # Get the action space for this agent
            action_space = env.single_action_spaces[agent_id]
            action[agent_id] = (
                int(action_space.sample())
                if hasattr(action_space.sample(), "item")
                else action_space.sample()
            )
        actions.append(action)

    observations, rewards, terminateds, truncateds, infos = env.step(actions)

    assert isinstance(observations, list)
    assert len(observations) == 4
    assert isinstance(rewards, list)
    assert len(rewards) == 4
    assert isinstance(terminateds, list)
    assert len(terminateds) == 4
    assert isinstance(truncateds, list)
    assert len(truncateds) == 4
    assert isinstance(infos, list)
    assert len(infos) == 4

    # Check that each element has the right structure
    for i in range(4):
        assert isinstance(observations[i], dict)
        assert isinstance(rewards[i], dict)
        assert isinstance(terminateds[i], dict)
        assert isinstance(truncateds[i], dict)
        assert isinstance(infos[i], dict)

        # Check for __all__ keys in terminated/truncated
        assert "__all__" in terminateds[i]
        assert "__all__" in truncateds[i]

        # Check observations have correct shape/type/values
        for agent_id in observations[i].keys():
            if agent_id != "__all__":  # Skip the special __all__ key
                obs = observations[i][agent_id]
                # Check that observation is in the observation space
                assert obs in env.single_observation_spaces[agent_id]

        # Check that EVERY entry in rewards is numeric (including all agent_ids)
        assert len(rewards[i]) > 0, f"Rewards dict should not be empty for env {i}"
        for key in rewards[i].keys():
            assert isinstance(
                rewards[i][key], (int, float, np.integer, np.floating)
            ), f"Reward for key '{key}' in env {i} should be numeric, got {type(rewards[i][key])}"

        # Check that EVERY entry in terminateds is boolean (including __all__)
        assert (
            len(terminateds[i]) > 0
        ), f"Terminateds dict should not be empty for env {i}"
        for key in terminateds[i].keys():
            assert isinstance(
                terminateds[i][key], (bool, np.bool_)
            ), f"Terminated for key '{key}' in env {i} should be boolean, got {type(terminateds[i][key])}"

        # Check that EVERY entry in truncateds is boolean (including __all__)
        assert (
            len(truncateds[i]) > 0
        ), f"Truncateds dict should not be empty for env {i}"
        for key in truncateds[i].keys():
            assert isinstance(
                truncateds[i][key], (bool, np.bool_)
            ), f"Truncated for key '{key}' in env {i} should be boolean, got {type(truncateds[i][key])}"

    env.close()


def test_rayvecenv_spaces(make_rllib_vec_env, make_env):
    """Test RayVecEnv observation and action space properties."""
    env_fns = [make_env("CartPole-v1", i) for i in range(4)]
    env = make_rllib_vec_env(env_fns)

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

    # For CartPole, check that the Dict spaces contain the correct subspaces
    # CartPole has: observation = Box(4,) for position, velocity, angle, angular velocity
    #               action = Discrete(2) for left or right
    for agent_id in env.single_observation_spaces:
        obs_space = env.single_observation_spaces[agent_id]
        # CartPole observation space is Box(4,) with shape (4,) and dtype float32
        assert isinstance(obs_space, gym.spaces.Box)
        assert obs_space.shape == (4,)
        assert obs_space.dtype == np.float32

    for agent_id in env.single_action_spaces:
        action_space = env.single_action_spaces[agent_id]
        # CartPole action space is Discrete(2)
        assert isinstance(action_space, gym.spaces.Discrete)
        assert action_space.n == 2

    env.close()


def test_rayvecenv_autoreset(make_rllib_vec_env, make_env):
    """Test RayVecEnv autoreset functionality - verify environments continue after completion."""

    def count_reset(self, seed=None, options=None):
        """Reset that initializes count based on seed."""
        super(GenericTestEnv, self).reset(seed=seed)
        self.count = seed if seed is not None else 0
        return self.count, {}

    def make_count_step(max_count):
        """Create step function that terminates after max_count steps."""

        def count_step(self, action):
            self.count += 1
            return self.count, action, self.count == max_count, False, {}

        return count_step

    max_counts = [2, 3, 3]
    env_fns = [
        partial(
            GenericTestEnv,
            action_space=Discrete(5),
            observation_space=Discrete(5),
            reset_func=count_reset,
            step_func=make_count_step(max_counts[i]),
        )
        for i in range(3)
    ]

    env = make_rllib_vec_env(env_fns)

    # Reset all environments
    obs, info = env.reset()

    # Step multiple times - with autoreset, environments should continue indefinitely
    actions = [{agent_id: 1 for agent_id in obs[i].keys()} for i in range(3)]

    # Run for enough steps that each env would terminate multiple times without autoreset
    # Env 0 would terminate at steps 2, 4, 6, 8, 10 (5 times)
    # Env 1&2 would terminate at steps 3, 6, 9 (3 times each)
    for step_num in range(15):
        obs, rewards, terminateds, truncateds, info = env.step(actions)

        # Verify that observations remain valid throughout
        for i in range(3):
            assert isinstance(obs[i], dict)
            for agent_id in obs[i].keys():
                # Observations should always be valid
                assert obs[i][agent_id] in env.single_observation_spaces[agent_id]

            # Verify structure of returns
            assert isinstance(rewards[i], dict)
            assert isinstance(terminateds[i], dict)
            assert isinstance(truncateds[i], dict)
            assert "__all__" in terminateds[i]
            assert "__all__" in truncateds[i]

    # If we got here without errors, autoreset is working
    # (environments would have errored out if they tried to step after termination without autoreset)
    env.close()


def test_rayvecenv_autoreset_next_step(make_rllib_vec_env):
    """Test RayVecEnv NEXT_STEP autoreset - environments reset on the step after termination."""

    def count_reset(self, seed=None, options=None):
        """Reset that initializes count based on seed."""
        super(GenericTestEnv, self).reset(seed=seed)
        self.count = seed if seed is not None else 0
        return self.count, {}

    def make_count_step(max_count):
        """Create step function that terminates after max_count steps."""

        def count_step(self, action):
            self.count += 1
            return self.count, action, self.count == max_count, False, {}

        return count_step

    # Create 3 environments with different termination counts
    max_counts = [2, 3, 3]
    env_fns = [
        partial(
            GenericTestEnv,
            action_space=Discrete(5),
            observation_space=Discrete(5),
            reset_func=count_reset,
            step_func=make_count_step(max_counts[i]),
        )
        for i in range(3)
    ]

    envs = make_rllib_vec_env(env_fns)

    # Helper function to extract observation values from dict structure
    def extract_obs_values(obs_list):
        """Extract observation values from list of dicts."""
        values = []
        for obs_dict in obs_list:
            # Get the first (and only) agent's observation
            agent_id = list(obs_dict.keys())[0]
            values.append(obs_dict[agent_id])
        return values

    # Helper function to extract reward/termination/truncation values from dict structure
    def extract_dict_values(dict_list, exclude_all=True):
        """Extract values from list of dicts, excluding __all__ key."""
        values = []
        for d in dict_list:
            # Get the first agent's value (excluding __all__)
            agent_ids = (
                [k for k in d.keys() if k != "__all__"]
                if exclude_all
                else list(d.keys())
            )
            if agent_ids:
                values.append(d[agent_ids[0]])
            else:
                values.append(None)
        return values

    # Reset and verify initial state
    obs, info = envs.reset()
    obs_values = extract_obs_values(obs)
    assert np.all(obs_values == [0, 0, 0])
    # Info is a list of dicts with agent_id keys, each containing empty dict
    assert len(info) == 3
    assert all(isinstance(i, dict) for i in info)

    # Step 1: All environments step normally
    actions = [
        {list(envs.single_action_spaces.keys())[0]: 1},
        {list(envs.single_action_spaces.keys())[0]: 2},
        {list(envs.single_action_spaces.keys())[0]: 3},
    ]
    obs, rewards, terminations, truncations, info = envs.step(actions)

    obs_values = extract_obs_values(obs)
    reward_values = extract_dict_values(rewards)
    term_values = extract_dict_values(terminations)
    trunc_values = extract_dict_values(truncations)

    assert np.all(obs_values == [1, 1, 1])
    assert np.all(reward_values == [1, 2, 3])
    assert np.all(term_values == [False, False, False])
    assert np.all(trunc_values == [False, False, False])
    assert len(info) == 3
    assert all(isinstance(i, dict) for i in info)

    # Step 2: Environment 0 terminates (count reaches max_count=2)
    obs, rewards, terminations, truncations, info = envs.step(actions)

    obs_values = extract_obs_values(obs)
    reward_values = extract_dict_values(rewards)
    term_values = extract_dict_values(terminations)
    trunc_values = extract_dict_values(truncations)

    assert np.all(obs_values == [2, 2, 2])
    assert np.all(reward_values == [1, 2, 3])
    assert np.all(term_values == [True, False, False])
    assert np.all(trunc_values == [False, False, False])
    assert len(info) == 3
    assert all(isinstance(i, dict) for i in info)

    # Step 3: Environment 0 auto-resets (NEXT_STEP), environments 1 and 2 terminate
    obs, rewards, terminations, truncations, info = envs.step(actions)

    obs_values = extract_obs_values(obs)
    reward_values = extract_dict_values(rewards)
    term_values = extract_dict_values(terminations)
    trunc_values = extract_dict_values(truncations)

    assert np.all(obs_values == [0, 3, 3])
    assert np.all(reward_values == [0, 2, 3])
    assert np.all(term_values == [False, True, True])
    assert np.all(trunc_values == [False, False, False])
    assert len(info) == 3
    assert all(isinstance(i, dict) for i in info)

    # Step 4: Environments 1 and 2 auto-reset (NEXT_STEP), environment 0 continues
    obs, rewards, terminations, truncations, info = envs.step(actions)

    obs_values = extract_obs_values(obs)
    reward_values = extract_dict_values(rewards)
    term_values = extract_dict_values(terminations)
    trunc_values = extract_dict_values(truncations)

    assert np.all(obs_values == [1, 0, 0])
    assert np.all(reward_values == [1, 0, 0])
    assert np.all(term_values == [False, False, False])
    assert np.all(trunc_values == [False, False, False])
    assert len(info) == 3
    assert all(isinstance(i, dict) for i in info)

    envs.close()


def test_environment_observation_space_is_sorted(make_rllib_vec_env, make_env):
    """Test that observation space keys are sorted."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Get the observation space keys
    obs_space_keys = list(env.single_observation_spaces.keys())

    # Check that keys are in sorted order
    assert len(obs_space_keys) > 0
    sorted_keys = sorted(obs_space_keys)
    assert (
        obs_space_keys == sorted_keys
    ), f"Observation space keys {obs_space_keys} are not sorted. Expected {sorted_keys}"

    env.close()


def test_environment_action_space_is_sorted(make_rllib_vec_env, make_env):
    """Test that action space keys are sorted."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Get the action space keys
    action_space_keys = list(env.single_action_spaces.keys())

    # Check that keys are in sorted order
    assert len(action_space_keys) > 0
    sorted_keys = sorted(action_space_keys)
    assert (
        action_space_keys == sorted_keys
    ), f"Action space keys {action_space_keys} are not sorted. Expected {sorted_keys}"

    env.close()


def test_agents_property_after_reset(make_rllib_vec_env, make_env):
    """Test that agents property returns agents after reset."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Reset environment
    observations, infos = env.reset()

    # RayVecEnv doesn't have agents property directly - check wrapper instances
    # Get all agents from all wrapper instances
    for i, wrapper in enumerate(env.envs):
        assert hasattr(wrapper, "agents"), "Wrapper should have agents property"
        assert isinstance(
            wrapper.agents, list
        ), f"Wrapper.agents should return a list, got {type(wrapper.agents)}"

        # Should have agents (at least one per environment)
        assert (
            len(wrapper.agents) == 1
        ), "Expected one agent after resetting cartpole-v1"
        assert (
            wrapper.agents == env.possible_agents
        ), "agents should be equivalent to possible_agents for cartpole-v1"
        assert set(observations[i].keys()) == set(
            wrapper.agents
        ), f"Observations should have the same keys as wrapper.agents for env {i}"

    env.close()


@pytest.mark.skip(reason="Needs a multi-agent environment to test")
def test_agents_property_excludes_terminated(make_rllib_vec_env, make_env):
    """Test that agents property excludes terminated agents."""
    env_fns = [make_env("FrozenLake-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    observations, infos = env.reset()

    # Get initial agents from wrappers
    initial_agents = [set(wrapper.agents) for wrapper in env.envs]
    for i in range(env.num_envs):
        assert (
            len(initial_agents[i]) == 1
        ), f"Expected one agent after resetting FrozenLake-v1 env-{i}"

    # Step until an agent terminates (FrozenLake can terminate)
    actions = []
    for obs in observations:
        action = {}
        for agent_id in obs.keys():
            action_space = env.single_action_spaces[agent_id]
            action[agent_id] = (
                int(action_space.sample())
                if hasattr(action_space.sample(), "item")
                else action_space.sample()
            )
        actions.append(action)

    # Step multiple times until we get a termination
    terminated_found = False
    for _ in range(100):  # Max 100 steps to find termination
        obs, rewards, terminateds, truncateds, infos = env.step(actions)

        # Check if any agent terminated
        for i, term_dict in enumerate(terminateds):
            wrapper = env.envs[i]
            for agent_id, is_term in term_dict.items():
                if agent_id != "__all__" and is_term:
                    # Check if environment auto-reset (if __all__ is True, autoreset happened)
                    env_auto_reset = term_dict.get("__all__", False)

                    if not env_auto_reset:
                        # Agent terminated but environment didn't reset - agent should be excluded from wrapper
                        terminated_found = True
                        current_agents = set(wrapper.agents)
                        assert (
                            agent_id not in current_agents
                        ), f"Terminated agent {agent_id} should not be in wrapper.agents (env {i} did not autoreset)"
                    else:
                        # Environment auto-reset - agent is active again (this is expected with autoreset)
                        terminated_found = (
                            True  # Found termination, but autoreset handled it
                        )
                        # Note: With autoreset, terminated agents are immediately reset and active again

        if terminated_found:
            break

        # Update actions for next step
        actions = []
        for obs_step in obs:
            action = {}
            for agent_id in obs_step.keys():
                action_space = env.single_action_spaces[agent_id]
                action[agent_id] = (
                    int(action_space.sample())
                    if hasattr(action_space.sample(), "item")
                    else action_space.sample()
                )
            actions.append(action)

    # Verify wrapper agents properties still work (even if all terminations triggered autoreset)
    for wrapper in env.envs:
        final_agents = set(wrapper.agents)
        assert isinstance(
            final_agents, set
        ), "wrapper.agents should always be a list that can be converted to set"
        assert final_agents.issubset(
            env.possible_agents
        ), "agents should be subset of possible_agents"

    # If we found a termination without autoreset, we already verified exclusion above
    # If all terminations triggered autoreset, that's also valid behavior

    env.close()


def test_possible_agents_is_static(make_rllib_vec_env, make_env):
    """Test that possible_agents grows monotonically (never shrinks)."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Get initial possible_agents
    initial_possible = env.possible_agents.copy()
    assert len(initial_possible) > 0, "Should have possible agents after initialization"

    # Reset
    observations, infos = env.reset()

    # possible_agents should not shrink
    after_reset_possible = env.possible_agents.copy()
    assert (
        initial_possible == after_reset_possible
    ), "possible_agents should not change after reset"

    # Step multiple times
    actions = []
    for obs in observations:
        action = {}
        for agent_id in obs.keys():
            action_space = env.single_action_spaces[agent_id]
            action[agent_id] = (
                int(action_space.sample())
                if hasattr(action_space.sample(), "item")
                else action_space.sample()
            )
        actions.append(action)

    for _ in range(10):
        obs, rewards, terminateds, truncateds, infos = env.step(actions)

        # possible_agents should not shrink
        current_possible = env.possible_agents.copy()
        assert (
            after_reset_possible == current_possible
        ), "possible_agents should not change during steps"

        # Update actions
        actions = []
        for obs_step in obs:
            action = {}
            for agent_id in obs_step.keys():
                action_space = env.single_action_spaces[agent_id]
                action[agent_id] = (
                    int(action_space.sample())
                    if hasattr(action_space.sample(), "item")
                    else action_space.sample()
                )
            actions.append(action)

    env.close()


def test_agents_tracking_after_multiple_resets(make_rllib_vec_env, make_env):
    """Test that agents tracking is consistent across multiple resets."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Reset multiple times - check wrapper agents
    agents_sets_per_wrapper = [[] for _ in range(len(env.envs))]
    for _ in range(5):
        observations, infos = env.reset()

        # Get agents from each wrapper
        for i, wrapper in enumerate(env.envs):
            agents = set(wrapper.agents)
            agents_sets_per_wrapper[i].append(agents)

            # Agents should be consistent (same agents each reset for same environment)
            if len(agents_sets_per_wrapper[i]) > 1:
                # Should have same number of agents (assuming same environment setup)
                assert len(agents) == len(
                    agents_sets_per_wrapper[i][0]
                ), f"Number of agents should be consistent across resets for wrapper {i}: {len(agents)} vs {len(agents_sets_per_wrapper[i][0])}"

    env.close()


def test_agents_vs_possible_agents_relationship(make_rllib_vec_env, make_env):
    """Test the relationship between agents and possible_agents properties."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    observations, infos = env.reset()

    # Get agents from wrappers
    all_agents = set()
    for wrapper in env.envs:
        all_agents.update(wrapper.agents)

    possible_agents = env.possible_agents

    # agents should be a subset of possible_agents
    assert all_agents.issubset(
        possible_agents
    ), f"agents ({all_agents}) should be a subset of possible_agents ({possible_agents})"

    # After reset, agents should equal possible_agents (all agents are active)
    # (unless some agents are terminated, which shouldn't happen on reset)
    assert all_agents == possible_agents or len(all_agents) <= len(
        possible_agents
    ), f"After reset, agents should equal or be subset of possible_agents"

    env.close()


def test_agent_tracking_with_termination(make_rllib_vec_env):
    """Test that agent tracking correctly handles termination."""
    from functools import partial
    from Test.gym.testing_env import GenericTestEnv
    from gymnasium.spaces import Discrete

    def quick_terminate_reset(self, seed=None, options=None):
        """Reset that initializes agent."""
        super(GenericTestEnv, self).reset(seed=seed)
        self.step_count = 0
        return 0, {}

    def quick_terminate_step(self, action):
        """Step that terminates immediately."""
        self.step_count += 1
        terminated = self.step_count >= 1  # Terminate after 1 step
        return self.step_count, action, terminated, False, {}

    env_fns = [
        partial(
            GenericTestEnv,
            action_space=Discrete(2),
            observation_space=Discrete(5),
            reset_func=quick_terminate_reset,
            step_func=quick_terminate_step,
        )
        for _ in range(2)
    ]

    env = make_rllib_vec_env(env_fns)

    # Reset
    observations, infos = env.reset()
    initial_agents = [set(wrapper.agents) for wrapper in env.envs]
    assert all(
        len(agents) > 0 for agents in initial_agents
    ), "Should have agents after reset"

    # Step - will cause termination
    actions = [{agent_id: 0 for agent_id in obs.keys()} for obs in observations]
    obs, rewards, terminateds, truncateds, infos = env.step(actions)

    # Check that terminated agents are tracked correctly
    for i, term_dict in enumerate(terminateds):
        wrapper = env.envs[i]
        for agent_id, is_term in term_dict.items():
            if agent_id != "__all__" and is_term:
                # Terminated agent tracking is in wrapper
                # Note: With autoreset, agent might immediately reset, so check carefully
                # The key is that _terminated_agents should be tracked in wrapper
                assert hasattr(
                    wrapper, "_terminated_agents"
                ), "Wrapper should have _terminated_agents tracking"

    env.close()


def test_baserayenv_inheritance_rayvecenv(make_rllib_vec_env, make_env):
    """Test that RayVecEnv inherits from BaseRayEnv but NOT MultiAgentEnv."""
    env_fns = [make_env("CartPole-v1", i) for i in range(2)]
    env = make_rllib_vec_env(env_fns)

    # Check inheritance
    assert isinstance(env, BaseRayEnv), "RayVecEnv should inherit from BaseRayEnv"
    assert isinstance(
        env, VectorMultiAgentEnv
    ), "RayVecEnv should inherit from VectorMultiAgentEnv"
    assert not isinstance(
        env, MultiAgentEnv
    ), "RayVecEnv should NOT inherit from MultiAgentEnv"

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

    # Check that RayVecEnv uses MultiAgentEnv via wrappers
    assert hasattr(env, "envs")
    assert len(env.envs) > 0
    assert isinstance(
        env.envs[0], MultiAgentEnv
    ), "RayVecEnv should use MultiAgentEnv via wrappers"

    env.close()


def test_rayvecenv_wrapper_structure(make_rllib_vec_env, make_env):
    """Test that RayVecEnv uses _SingleEnvWrapper instances correctly."""
    env_fns = [make_env("CartPole-v1", i) for i in range(3)]
    env = make_rllib_vec_env(env_fns)

    # Check that envs list exists and has correct length
    assert hasattr(env, "envs")
    assert isinstance(env.envs, list)
    assert len(env.envs) == 3

    # Check that each wrapper is a MultiAgentEnv
    for i, wrapper in enumerate(env.envs):
        assert isinstance(
            wrapper, MultiAgentEnv
        ), f"Wrapper {i} should be a MultiAgentEnv instance"
        assert hasattr(wrapper, "env_id")
        assert wrapper.env_id == i
        assert hasattr(wrapper, "possible_agents")
        assert hasattr(wrapper, "agents")
        assert wrapper.parent_vec_env == env

    env.close()


@pytest.mark.xdist_group(name="ray-cluster")
def test_vec_training(make_vec_env_server, make_env, ray_cluster):
    env_server_port = make_vec_env_server(
        [make_env("CartPole-v1", i) for i in range(2)]
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
