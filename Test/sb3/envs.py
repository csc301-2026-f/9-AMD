# Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

"""Minimal Gymnasium environments for Stable-Baselines3 Schola tests."""

import gymnasium as gym
import numpy as np
from gymnasium.spaces import Box, Dict, Discrete, MultiBinary
from gymnasium.spaces.utils import flatten, unflatten, flatten_space
from schola.sb3.utils import split_value, merge_spaces


class DictActionEnv(gym.Env):
    """A simple test environment with dictionary action space."""

    def __init__(self, action_space: Dict, expect_flat_action: bool = False):
        super().__init__()
        self.observation_space = Box(low=0, high=1, shape=(4,), dtype=np.float32)
        self.action_space = action_space
        self.max_steps = 100
        self.flat_action = expect_flat_action
        all_action_spaces = list(action_space.spaces.values())
        if len(all_action_spaces) == 0:
            self.flat_action_space = None
        elif len(all_action_spaces) == 1:
            self.flat_action_space = all_action_spaces[0]
        else:
            self.flat_action_space = merge_spaces(*all_action_spaces)

    def validate_action(self, action: np.ndarray | Dict):
        """Validate the action is correct."""
        if self.flat_action:
            assert self.flat_action_space.contains(
                action
            ), f"Action {action} is not in flattened action space {self.flat_action_space}"
        else:
            assert self.action_space.contains(
                action
            ), f"Action {action} is not in action space {self.action_space}"

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        self.step_count = 0
        return self.observation_space.sample(), {}

    def step(self, action):
        """Accept dictionary action with 'left_motor' and 'right_motor' keys."""
        # Verify action structure
        self.validate_action(action)

        self.step_count += 1
        obs = self.observation_space.sample()
        reward = 1.0
        terminated = False
        truncated = self.step_count >= self.max_steps
        info = {}

        return obs, reward, terminated, truncated, info


class DictActionBoxEnv(DictActionEnv):
    """A simple test environment with dictionary action space containing Box actions."""

    def __init__(self, expect_flat_action: bool = False):
        action_space = Dict(
            spaces={
                "left_motor": Box(low=-1, high=1, shape=(2,), dtype=np.float32),
                "right_motor": Box(low=-1, high=1, shape=(2,), dtype=np.float32),
            }
        )
        super().__init__(action_space, expect_flat_action)


class DictActionDiscreteEnv(DictActionEnv):
    """A simple test environment with dictionary action space containing Discrete actions."""

    def __init__(self, expect_flat_action: bool = False):
        action_space = Dict(
            spaces={
                "button": Discrete(3),
                "direction": Discrete(4),
            }
        )
        super().__init__(action_space, expect_flat_action)


class DictActionMixedEnv(DictActionEnv):
    """A simple test environment with dictionary action space containing mixed action types."""

    def __init__(self, expect_flat_action: bool = False):
        action_space = Dict(
            spaces={
                "continuous": Box(low=-1, high=1, shape=(3,), dtype=np.float32),
                "discrete": Discrete(5),
            }
        )
        super().__init__(action_space, expect_flat_action)

    # No action validation because this is not supported currently


class DictActionMultiBinaryEnv(DictActionEnv):
    """A simple test environment with dictionary action space containing MultiBinary actions."""

    def __init__(self, expect_flat_action: bool = False):
        action_space = Dict(
            spaces={
                "switches": MultiBinary(3),
                "lights": MultiBinary(2),
            }
        )
        super().__init__(action_space, expect_flat_action)


class DictActionEmptyEnv(DictActionEnv):

    def __init__(self, expect_flat_action: bool = False):
        super().__init__(Dict(spaces={}), expect_flat_action)


def make_dict_action_env(env_class, expect_flat_action: bool = True):
    """Make a dictionary action environment with the given class and expect flat action flag. Use expect_flat_action=True when the using to test DummyVecEnv.
    Use expect_flat_action=False when the using to test as an environment hosted on a gRPC server.
    """

    def _inner():
        return env_class(expect_flat_action)

    return _inner
