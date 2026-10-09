"""Tests for diferent auto-reset modes in Schola Python"""

# The below code is adapted from https://github.com/Farama-Foundation/Gymnasium/blob/main/tests/vector/test_autoreset_mode.py
"""
The MIT License

Copyright (c) 2016 OpenAI
Copyright (c) 2022 Farama Foundation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""
# Modifications Copyright (c) 2025-2026 Advanced Micro Devices, Inc. All Rights Reserved.

from functools import partial

import numpy as np
import pytest

import gymnasium as gym
from gymnasium import VectorizeMode
from gymnasium.spaces import Discrete
from gymnasium.utils.env_checker import data_equivalence
from gymnasium.vector import AsyncVectorEnv, SyncVectorEnv
from gymnasium.vector.vector_env import AutoresetMode

from schola.gym.env import GymVectorEnv
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor

from Test.testing_spaces import TESTING_SPACES, TESTING_SPACES_IDS
from Test.gym.testing_env import GenericTestEnv
from functools import partial


def count_reset(
    self: GenericTestEnv, seed: int | None = None, options: dict | None = None
):
    super(GenericTestEnv, self).reset(seed=seed)

    self.count = seed if seed is not None else 0
    return self.count, {}


def make_count_step(max_count):
    def count_step(self: GenericTestEnv, action):
        self.count += 1
        return self.count, action, self.count == max_count, False, {}

    return count_step


def test_autoreset_next_step(make_vec_env_server):

    max_counts = [2, 3, 3]
    env_server_port = make_vec_env_server(
        [
            partial(
                GenericTestEnv,
                action_space=Discrete(5),
                observation_space=Discrete(5),
                reset_func=count_reset,
                step_func=make_count_step(max_counts[i]),
            )
            for i in range(3)
        ]
    )
    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=AutoresetMode.NEXT_STEP)

    assert envs.metadata["autoreset_mode"] == AutoresetMode.NEXT_STEP

    obs, info = envs.reset()
    assert np.all(obs == [0, 0, 0])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 1, 1])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [False, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [2, 2, 2])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [True, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [0, 3, 3])
    assert np.all(rewards == [0, 2, 3])
    assert np.all(terminations == [False, True, True])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 0, 0])
    assert np.all(rewards == [1, 0, 0])
    assert np.all(terminations == [False, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    envs.close()


def test_autoreset_within_step(make_vec_env_server):
    max_counts = [2, 3, 3]
    env_server_port = make_vec_env_server(
        [
            partial(
                GenericTestEnv,
                action_space=Discrete(5),
                observation_space=Discrete(5),
                reset_func=count_reset,
                step_func=make_count_step(max_counts[i]),
            )
            for i in range(3)
        ]
    )

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=AutoresetMode.SAME_STEP)

    assert envs.metadata["autoreset_mode"] == AutoresetMode.SAME_STEP

    obs, info = envs.reset()
    assert np.all(obs == [0, 0, 0])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 1, 1])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [False, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [0, 2, 2])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [True, False, False])
    assert np.all(truncations == [False, False, False])
    assert data_equivalence(
        info,
        {
            "final_obs": np.array([2, None, None], dtype=object),
            "final_info": {},
            "_final_obs": np.array([True, False, False]),
            "_final_info": np.array([True, False, False]),
        },
    )

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 0, 0])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [False, True, True])
    assert np.all(truncations == [False, False, False])
    assert data_equivalence(
        info,
        {
            "final_obs": np.array([None, 3, 3], dtype=object),
            "final_info": {},
            "_final_obs": np.array([False, True, True]),
            "_final_info": np.array([False, True, True]),
        },
    )

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [0, 1, 1])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [True, False, False])
    assert np.all(truncations == [False, False, False])
    assert data_equivalence(
        info,
        {
            "final_obs": np.array([2, None, None], dtype=object),
            "final_info": {},
            "_final_obs": np.array([True, False, False]),
            "_final_info": np.array([True, False, False]),
        },
    )

    envs.close()


@pytest.mark.skip(reason="Feature not implemented yet")
def test_autoreset_disabled(make_vec_env_server):
    max_counts = [2, 3, 3]
    env_server_port = make_vec_env_server(
        [
            partial(
                GenericTestEnv,
                action_space=Discrete(5),
                observation_space=Discrete(5),
                reset_func=count_reset,
                step_func=make_count_step(max_counts[i]),
            )
            for i in range(3)
        ]
    )

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=AutoresetMode.DISABLED)

    assert envs.metadata["autoreset_mode"] == AutoresetMode.DISABLED

    obs, info = envs.reset()
    assert np.all(obs == [0, 0, 0])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 1, 1])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [False, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [2, 2, 2])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [True, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, info = envs.reset(options={"reset_mask": terminations})
    assert np.all(obs == [0, 2, 2])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [1, 3, 3])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [False, True, True])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    obs, info = envs.reset(options={"reset_mask": terminations})
    assert np.all(obs == [1, 0, 0])
    assert info == {}

    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert np.all(obs == [2, 1, 1])
    assert np.all(rewards == [1, 2, 3])
    assert np.all(terminations == [True, False, False])
    assert np.all(truncations == [False, False, False])
    assert info == {}

    envs.close()


@pytest.mark.parametrize(
    "autoreset_mode",
    [AutoresetMode.NEXT_STEP, AutoresetMode.DISABLED, AutoresetMode.SAME_STEP],
)
def test_autoreset_metadata(make_vec_env_server, autoreset_mode):
    env_server_port = make_vec_env_server([partial(GenericTestEnv) for _ in range(2)])

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=autoreset_mode)

    assert envs.metadata["autoreset_mode"] == autoreset_mode
    envs.close()

    env_server_port = make_vec_env_server([partial(GenericTestEnv) for _ in range(2)])

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=autoreset_mode.value)

    assert envs.metadata["autoreset_mode"] == autoreset_mode
    envs.close()


def count_reset_obs(
    self: GenericTestEnv, seed: int | None = None, options: dict | None = None
):
    super(GenericTestEnv, self).reset(seed=seed)

    self.count = seed if seed is not None else 0
    return self.observation_space.sample(), {}


def make_count_step_obs(max_count):
    def count_step_obs(self: GenericTestEnv, action):
        self.count += 1

        return (
            self.observation_space.sample(),
            action,
            self.count == max_count,
            False,
            {},
        )

    return count_step_obs


@pytest.mark.parametrize("obs_space", TESTING_SPACES, ids=TESTING_SPACES_IDS)
def test_same_step_final_obs(make_vec_env_server, obs_space):
    max_counts = [2, 3, 3]
    env_server_port = make_vec_env_server(
        [
            partial(
                GenericTestEnv,
                action_space=Discrete(5),
                observation_space=obs_space,
                reset_func=count_reset_obs,
                step_func=make_count_step_obs(max_counts[i]),
            )
            for i in range(3)
        ]
    )

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    envs = GymVectorEnv(simulator, protocol, autoreset_mode=AutoresetMode.SAME_STEP)

    assert envs.metadata["autoreset_mode"] == AutoresetMode.SAME_STEP

    envs.reset()
    envs.step([1, 2, 3])
    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert info["final_obs"][0] in envs.single_observation_space
    obs, rewards, terminations, truncations, info = envs.step([1, 2, 3])
    assert info["final_obs"][1] in envs.single_observation_space
    assert info["final_obs"][2] in envs.single_observation_space
