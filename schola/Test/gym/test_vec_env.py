"""Test the Gymnasium Vector Env Implementation."""

# The below code is adapted from https://github.com/Farama-Foundation/Gymnasium/blob/main/tests/vector/test_sync_vector_env.py
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
# Modifications Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.


import re

import numpy as np
import pytest

from gymnasium.envs.registration import EnvSpec
from gymnasium.spaces import Box, Discrete, MultiDiscrete, Tuple
from gymnasium.vector import SyncVectorEnv
from Test.gym.vec_utils import (
    CustomSpace,
    assert_rng_equal,
    make_custom_space_env,
    make_env,
)
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from schola.gym.env import GymVectorEnv
from gymnasium.vector.vector_env import AutoresetMode

all_testing_env_specs = []


def assert_gymnasium_vector_infos(infos: dict, num_envs: int) -> None:
    """Assert ``infos`` matches Gymnasium ``VectorEnv._add_info`` layout.

    Each non-mask key ``k`` has a sibling mask ``_{k}`` (bool ndarray, shape
    ``(num_envs,)``). Leaf values are ndarrays whose first axis is ``num_envs``.
    Nested dict values are validated recursively with the same rules.
    """
    assert isinstance(infos, dict)
    assert all(isinstance(k, str) for k in infos), (
        "top-level keys must be str (info names or _mask names), not env ids"
    )
    data_keys = {k for k in infos if not k.startswith("_")}
    mask_keys = {k for k in infos if k.startswith("_")}
    expected_masks = {f"_{k}" for k in data_keys}
    assert mask_keys == expected_masks, (
        "every data key must have exactly one ``_<key>`` mask and vice versa; "
        f"symmetric difference: {mask_keys ^ expected_masks}"
    )
    for k in data_keys:
        v = infos[k]
        mask = infos[f"_{k}"]
        assert isinstance(mask, np.ndarray), k
        assert mask.dtype == np.bool_, k
        assert mask.shape == (num_envs,), k
        if isinstance(v, dict):
            assert_gymnasium_vector_infos(v, num_envs)
        else:
            assert isinstance(v, np.ndarray), k
            assert v.shape[0] == num_envs, (k, v.shape)


@pytest.fixture(scope="function")
def make_vec_env(make_vec_env_server):
    def _factory(env_funcs):
        env_server_port = make_vec_env_server(env_funcs)
        simulator = UnrealEditor()
        protocol = GrpcProtocol(url="localhost", port=env_server_port)
        return GymVectorEnv(simulator, protocol, autoreset_mode=AutoresetMode.NEXT_STEP)

    return _factory


def test_create_vector_env(make_vec_env):
    """Tests creating the vector environment."""
    env_fns = [make_env("FrozenLake-v1", i) for i in range(8)]
    env = make_vec_env(env_fns)

    assert env.num_envs == 8

    env.close()

    assert env.num_envs == 8


def test_reset_vector_env(make_vec_env):
    """Tests vector `reset` function."""
    env_fns = [make_env("CartPole-v1", i) for i in range(8)]
    env = make_vec_env(env_fns)
    observations, infos = env.reset()
    env.close()

    assert isinstance(env.observation_space, Box)
    assert isinstance(observations, np.ndarray)
    assert observations.dtype == env.observation_space.dtype
    assert observations.shape == (8,) + env.single_observation_space.shape
    assert observations.shape == env.observation_space.shape
    assert_gymnasium_vector_infos(infos, 8)

    del observations


def test_reset_mask_not_supported(make_vec_env):
    """Tests that `reset` with mask raises error."""
    env_fns = [make_env("CartPole-v1", i) for i in range(8)]
    env = make_vec_env(env_fns)

    with pytest.raises(NotImplementedError):
        env.reset(options={"reset_mask": [True] * 8})

    env.close()


def test_reset_mask_not_supported_options_list(make_vec_env):
    """Per-env options list must reject reset_mask like a single dict does."""
    n = 8
    env_fns = [make_env("CartPole-v1", i) for i in range(n)]
    env = make_vec_env(env_fns)
    options = [{}] * (n - 1) + [{"reset_mask": [True]}]
    with pytest.raises(NotImplementedError):
        env.reset(options=options)
    env.close()


def test_reset_options_list_per_unreal_env(make_vec_env):
    """Per-Unreal-env options list is forwarded when length matches num_envs."""
    n = 8
    env_fns = [make_env("CartPole-v1", i) for i in range(n)]
    env = make_vec_env(env_fns)
    options = [{"episode_id": str(i)} for i in range(n)]
    obs, infos = env.reset(options=options)
    assert obs.shape == (n,) + env.single_observation_space.shape
    assert len(env.reset_infos) == n
    assert_gymnasium_vector_infos(infos, n)
    env.close()


def test_reset_options_list_length_mismatch(make_vec_env):
    """Wrong list length raises ValueError with Unreal vs vector hint."""
    n = 8
    env_fns = [make_env("CartPole-v1", i) for i in range(n)]
    env = make_vec_env(env_fns)
    with pytest.raises(ValueError, match="Unreal"):
        env.reset(options=[{}] * (n - 1))
    env.close()


def test_reset_seed_list_length_mismatch(make_vec_env):
    """Seed list length must match Unreal sub-environment count."""
    n = 8
    env_fns = [make_env("CartPole-v1", i) for i in range(n)]
    env = make_vec_env(env_fns)
    with pytest.raises(ValueError, match="Unreal"):
        env.reset(seed=[0] * (n - 1))
    env.close()


@pytest.mark.parametrize("use_single_action_space", [True, False])
def test_step_vector_env(use_single_action_space, make_vec_env):
    """Test vector `steps` function."""
    env = make_vec_env([make_env("FrozenLake-v1", i) for i in range(8)])
    env.reset()

    assert isinstance(env.single_action_space, Discrete)
    assert isinstance(env.action_space, MultiDiscrete)

    if use_single_action_space:
        actions = [env.single_action_space.sample() for _ in range(8)]
    else:
        actions = env.action_space.sample()
    observations, rewards, terminations, truncations, infos = env.step(actions)

    env.close()

    assert isinstance(env.observation_space, MultiDiscrete)
    assert isinstance(observations, np.ndarray)
    assert observations.dtype == env.observation_space.dtype
    assert observations.shape == (8,) + env.single_observation_space.shape
    assert observations.shape == env.observation_space.shape

    assert isinstance(rewards, np.ndarray)
    assert isinstance(rewards[0], (float, np.floating))
    assert rewards.ndim == 1
    assert rewards.size == 8

    assert isinstance(terminations, np.ndarray)
    assert terminations.dtype == np.bool_
    assert terminations.ndim == 1
    assert terminations.size == 8

    assert isinstance(truncations, np.ndarray)
    assert truncations.dtype == np.bool_
    assert truncations.ndim == 1
    assert truncations.size == 8

    assert len(env.reset_infos) == 8
    assert_gymnasium_vector_infos(infos, 8)


@pytest.mark.skip("render not supported in schola envs yet")
def test_render_vector(make_vec_env):
    envs = make_vec_env(
        [make_env("CartPole-v1", i, render_mode="rgb_array") for i in range(3)]
    )
    assert envs.render_mode == "rgb_array"

    envs.reset()
    rendered_frames = envs.render()
    assert isinstance(rendered_frames, tuple)
    assert len(rendered_frames) == envs.num_envs
    assert all(isinstance(frame, np.ndarray) for frame in rendered_frames)

    envs = SyncVectorEnv([make_env("CartPole-v1", i) for i in range(3)])
    assert envs.render_mode is None


@pytest.mark.skip("call not supported in schola envs yet")
def test_call_vector_env(make_vec_env):
    """Test vector `call` on sub-environments."""
    env_fns = [
        make_env("CartPole-v1", i, render_mode="rgb_array_list") for i in range(4)
    ]

    env = make_vec_env(env_fns)
    _ = env.reset()
    images = env.call("render")
    gravity = env.call("gravity")

    env.close()

    assert isinstance(images, tuple)
    assert len(images) == 4
    for i in range(4):
        assert len(images[i]) == 1
        assert isinstance(images[i][0], np.ndarray)

    assert isinstance(gravity, tuple)
    assert len(gravity) == 4
    for i in range(4):
        assert isinstance(gravity[i], float)
        assert gravity[i] == 9.8


@pytest.mark.skip("set attr not supported in schola envs yet")
def test_set_attr_vector_env(make_vec_env):
    """Test vector `set_attr` function."""
    env_fns = [make_env("CartPole-v1", i) for i in range(4)]

    env = make_vec_env(env_fns)
    env.set_attr("gravity", [9.81, 3.72, 8.87, 1.62])
    gravity = env.get_attr("gravity")
    assert gravity == (9.81, 3.72, 8.87, 1.62)

    env.close()


@pytest.mark.skip()
def test_vector_env_seed(make_vec_env):
    """Test seeding for sync vector environments."""
    env = make_env("CartPole-v1", seed=123)()
    vector_env = make_vec_env([make_env("CartPole-v1", seed=123)])

    assert_rng_equal(env.action_space.np_random, vector_env.action_space.np_random)
    for _ in range(100):
        env_action = env.action_space.sample()
        vector_action = vector_env.action_space.sample()
        assert np.all(env_action == vector_action)

    env.close()


@pytest.mark.parametrize(
    "spec", all_testing_env_specs, ids=[spec.id for spec in all_testing_env_specs]
)
def test_vector_determinism(spec: EnvSpec, seed: int = 123, n: int = 3):
    """Check that for all environments, the sync vector envs produce the same action samples using the same seeds."""
    env_1 = make_vec_env([make_env(spec.id, seed=seed) for _ in range(n)])
    env_2 = make_vec_env([make_env(spec.id, seed=seed) for _ in range(n)])
    assert_rng_equal(env_1.action_space.np_random, env_2.action_space.np_random)

    for _ in range(100):
        env_1_samples = env_1.action_space.sample()
        env_2_samples = env_2.action_space.sample()
        assert np.all(env_1_samples == env_2_samples)

    env_1.close()
    env_2.close()
