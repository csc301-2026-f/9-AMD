# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
"""
Tests for schola.gym.utils module
"""

import pytest
import gymnasium as gym
from gymnasium.spaces import Dict, Box, Discrete
import numpy as np
from schola.gym.utils import PopActionWrapper


class DummyDictActionEnv(gym.Env):
    """A dummy environment with Dict action space for testing"""

    def __init__(self):
        super().__init__()
        self.observation_space = Box(low=0, high=1, shape=(4,), dtype=np.float32)
        self.action_space = Dict(
            {
                "action1": Box(low=-1, high=1, shape=(2,), dtype=np.float32),
                "action2": Discrete(3),
            }
        )
        self.step_count = 0

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        self.step_count = 0
        return self.observation_space.sample(), {}

    def step(self, action):
        self.step_count += 1
        obs = self.observation_space.sample()
        reward = 1.0
        terminated = self.step_count >= 10
        truncated = False
        info = {"action_received": action}
        return obs, reward, terminated, truncated, info


class TestPopActionWrapper:
    """Tests for PopActionWrapper"""

    def test_wrapper_init(self):
        """Test that wrapper initializes correctly with Dict action space"""
        env = DummyDictActionEnv()
        wrapped = PopActionWrapper(env)

        # Should pop the first action from the dict
        assert wrapped.key == "action1"
        assert isinstance(wrapped.action_space, Box)
        assert wrapped.action_space.shape == (2,)

    def test_wrapper_init_non_dict_fails(self):
        """Test that wrapper fails with non-Dict action space"""
        env = gym.make("CartPole-v1")

        with pytest.raises(
            AssertionError, match="Action space must be a Dictionary Space"
        ):
            PopActionWrapper(env)

    def test_step_with_single_action(self):
        """Test that step correctly wraps action into dict"""
        env = DummyDictActionEnv()
        wrapped = PopActionWrapper(env)

        wrapped.reset()

        # Step with single action (not dict)
        action = wrapped.action_space.sample()
        obs, reward, terminated, truncated, info = wrapped.step(action)

        # Verify the action was wrapped correctly
        assert "action_received" in info
        assert "action1" in info["action_received"]
        assert np.array_equal(info["action_received"]["action1"], action)

    def test_observation_space_unchanged(self):
        """Test that observation space is not modified"""
        env = DummyDictActionEnv()
        wrapped = PopActionWrapper(env)

        assert wrapped.observation_space == env.observation_space

    def test_reset_works(self):
        """Test that reset works correctly"""
        env = DummyDictActionEnv()
        wrapped = PopActionWrapper(env)

        obs, info = wrapped.reset()

        assert wrapped.observation_space.contains(obs)
        assert isinstance(info, dict)

    def test_multiple_steps(self):
        """Test multiple steps work correctly"""
        env = DummyDictActionEnv()
        wrapped = PopActionWrapper(env)

        wrapped.reset()

        for i in range(5):
            action = wrapped.action_space.sample()
            obs, reward, terminated, truncated, info = wrapped.step(action)

            assert wrapped.observation_space.contains(obs)
            assert isinstance(reward, (int, float))
            assert isinstance(terminated, bool)
            assert isinstance(truncated, bool)

            if terminated or truncated:
                break
