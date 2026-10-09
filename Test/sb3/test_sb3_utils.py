# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
"""Tests for the SB3 utility functions and wrappers"""

import pytest
import gymnasium as gym
import numpy as np
from gymnasium.spaces import (
    Box,
    Discrete,
    MultiDiscrete,
    MultiBinary,
    Dict as DictSpace,
)
from stable_baselines3.common.vec_env import DummyVecEnv
from unittest.mock import Mock, MagicMock, patch
from schola.sb3.utils import (
    merge_spaces,
    RenderImagesWrapper,
    split_box_value,
    split_multibinary_value,
    split_multidiscrete_value,
    split_value,
)


class TestMergeSpaces:
    """Test suite for the merge_spaces singledispatch function."""

    def test_merge_box_1d_spaces(self):
        """Test merging 1D Box spaces."""
        space1 = Box(low=0, high=1, shape=(3,), dtype=np.float32)
        space2 = Box(low=0, high=1, shape=(2,), dtype=np.float32)
        space3 = Box(low=-1, high=1, shape=(4,), dtype=np.float32)

        merged = merge_spaces(space1, space2, space3)

        assert isinstance(merged, Box), f"Merged space is not a Box: {type(merged)}"
        assert merged.shape == (9,), f"Merged space has wrong shape: {merged.shape}"
        assert (
            merged.dtype == np.float32
        ), f"Merged space has wrong dtype: {merged.dtype}"

    def test_merge_box_2d_spaces(self):
        """Test merging 2D Box spaces (like images)."""
        space1 = Box(low=0, high=255, shape=(84, 84, 3), dtype=np.uint8)
        space2 = Box(low=0, high=255, shape=(84, 84, 1), dtype=np.uint8)

        merged = merge_spaces(space1, space2)

        assert isinstance(merged, Box), f"Merged space is not a Box: {type(merged)}"
        assert merged.shape == (
            84,
            84,
            4,
        ), f"Merged space has wrong shape: {merged.shape}"
        assert merged.dtype == np.uint8, f"Merged space has wrong dtype: {merged.dtype}"

    def test_merge_box_different_dtype_raises(self):
        """Test that merging Box spaces with different dtypes raises an error."""
        space1 = Box(low=0, high=1, shape=(3,), dtype=np.float32)
        space2 = Box(low=0, high=1, shape=(2,), dtype=np.float64)

        with pytest.raises(ValueError, match="same dtype"):
            merge_spaces(space1, space2)

    def test_merge_box_different_dimensions_raises(self):
        """Test that merging Box spaces with different dimensions raises an error."""
        space1 = Box(low=0, high=1, shape=(3,), dtype=np.float32)
        space2 = Box(low=0, high=1, shape=(2, 2), dtype=np.float32)

        with pytest.raises(ValueError, match="same number of dimensions"):
            merge_spaces(space1, space2)

    def test_merge_discrete_spaces(self):
        """Test merging Discrete spaces into MultiDiscrete."""
        space1 = Discrete(5)
        space2 = Discrete(3)
        space3 = Discrete(7)

        merged = merge_spaces(space1, space2, space3)

        assert isinstance(merged, MultiDiscrete)
        assert len(merged.nvec) == 3
        # Note: Based on the user's code, dims.append([s.n]) suggests each Discrete
        # gets wrapped in a list, so we need to check the implementation

    def test_merge_mixed_discrete_and_multidiscrete(self):
        """Test merging a mix of Discrete and MultiDiscrete spaces."""
        space1 = Discrete(5)
        space2 = MultiDiscrete([3, 4])
        space3 = Discrete(2)

        merged = merge_spaces(space1, space2, space3)

        assert isinstance(merged, MultiDiscrete)
        # Should have 1 (from space1) + 2 (from space2) + 1 (from space3) = 4 dimensions
        assert (
            len(merged.nvec) == 4
        ), f"Merged space has wrong number of dimensions: {len(merged.nvec)}"
        np.testing.assert_array_equal(merged.nvec, np.array([5, 3, 4, 2]))

    def test_merge_multibinary_spaces(self):
        """Test merging MultiBinary spaces."""
        space1 = MultiBinary(5)
        space2 = MultiBinary(3)
        space3 = MultiBinary(7)

        merged = merge_spaces(space1, space2, space3)

        assert isinstance(merged, MultiBinary)
        assert merged.n == 15  # 5 + 3 + 7

    def test_merge_multibinary_wrong_type_raises(self):
        """Test that merging MultiBinary with incompatible type raises an error."""
        space1 = MultiBinary(5)
        space2 = Box(low=0, high=1, shape=(3,), dtype=np.float32)

        with pytest.raises(TypeError, match="Cannot merge MultiBinary space"):
            merge_spaces(space1, space2)

    def test_merge_unsupported_space_raises(self):
        """Test that merging unsupported space types raises NotImplementedError."""
        space = gym.spaces.Text(max_length=10)

        with pytest.raises(NotImplementedError, match="Merge not implemented"):
            merge_spaces(space, space)

    def test_merge_three_box_spaces(self):
        """Test merging three Box spaces"""
        space1 = Box(low=-1, high=1, shape=(2,))
        space2 = Box(low=-2, high=2, shape=(3,))
        space3 = Box(low=-3, high=3, shape=(4,))

        merged = merge_spaces(space1, space2, space3)

        assert isinstance(merged, Box)
        assert merged.shape == (9,)

    def test_merge_single_box_space(self):
        """Merging a single Box space returns an equivalent Box."""
        space = Box(low=-1, high=2, shape=(5,), dtype=np.float32)
        merged = merge_spaces(space)
        assert isinstance(merged, Box)
        assert merged.shape == space.shape
        assert merged.dtype == space.dtype
        np.testing.assert_array_equal(merged.low, space.low)
        np.testing.assert_array_equal(merged.high, space.high)

    def test_merge_box_with_non_box_raises(self):
        """Box merge rejects non-Box spaces in the tail arguments."""
        with pytest.raises(TypeError, match="Cannot merge Box space"):
            merge_spaces(
                Box(low=0, high=1, shape=(2,), dtype=np.float32),
                Discrete(3),
            )

    def test_merge_discrete_with_non_discrete_raises(self):
        """Discrete merge rejects incompatible space types."""
        with pytest.raises(TypeError, match="Cannot merge Discrete or MultiDiscrete"):
            merge_spaces(
                Discrete(5),
                Box(low=0, high=1, shape=(2,), dtype=np.float32),
            )

    def test_merge_discrete_nvec_values(self):
        """Merged MultiDiscrete carries each Discrete dimension in order."""
        merged = merge_spaces(Discrete(5), Discrete(3), Discrete(7))
        assert isinstance(merged, MultiDiscrete)
        np.testing.assert_array_equal(merged.nvec, np.array([5, 3, 7]))


class TestSplitHelpers:
    """Tests for split_box_value, split_multibinary_value, split_multidiscrete_value, split_value."""

    def test_split_box_value_1d_roundtrip(self):
        a = Box(low=0, high=1, shape=(3,), dtype=np.float32)
        b = Box(low=-1, high=1, shape=(2,), dtype=np.float32)
        merged = merge_spaces(a, b)
        flat = np.array([[0.1, 0.2, 0.3, 0.4, 0.5]], dtype=np.float32)
        parts = split_box_value(flat, {"a": a, "b": b})
        assert parts["a"].shape == (1, 3)
        assert parts["b"].shape == (1, 2)
        np.testing.assert_allclose(parts["a"], flat[:, :3])
        np.testing.assert_allclose(parts["b"], flat[:, 3:])
        assert merged.contains(flat[0])

    def test_split_box_value_2d_image_like(self):
        a = Box(low=0, high=1, shape=(2, 2, 3), dtype=np.float32)
        b = Box(low=0, high=1, shape=(2, 2, 1), dtype=np.float32)
        flat = np.zeros((4, 2, 2, 4), dtype=np.float32)
        flat[..., :3] = 0.25
        flat[..., 3:] = 0.5
        parts = split_box_value(flat, {"a": a, "b": b})
        assert parts["a"].shape == (4, 2, 2, 3)
        assert parts["b"].shape == (4, 2, 2, 1)

    def test_split_box_value_accepts_gym_dict_space(self):
        a = Box(low=0, high=1, shape=(2,), dtype=np.float32)
        b = Box(low=0, high=1, shape=(3,), dtype=np.float32)
        spaces = DictSpace({"a": a, "b": b})
        flat = np.arange(10, dtype=np.float32).reshape(2, 5)
        parts = split_box_value(flat, spaces)
        assert parts["a"].shape == (2, 2)
        assert parts["b"].shape == (2, 3)

    def test_split_box_value_rejects_wrong_space_type(self):
        with pytest.raises(TypeError, match="Expected Box space"):
            split_box_value(
                np.zeros((1, 2), dtype=np.float32),
                {"x": MultiBinary(2)},  # type: ignore[arg-type]
            )

    def test_split_multibinary_roundtrip(self):
        m1 = MultiBinary(3)
        m2 = MultiBinary(2)
        merged = merge_spaces(m1, m2)
        flat = np.array([[1, 0, 1, 1, 0]], dtype=np.int8)
        parts = split_multibinary_value(flat, {"u": m1, "v": m2})
        assert parts["u"].shape == (1, 3)
        assert parts["v"].shape == (1, 2)
        np.testing.assert_array_equal(parts["u"], flat[:, :3])
        np.testing.assert_array_equal(parts["v"], flat[:, 3:])
        assert merged.contains(flat[0].astype(np.int8))

    def test_split_multibinary_rejects_wrong_space_type(self):
        with pytest.raises(TypeError, match="Expected MultiBinary space"):
            split_multibinary_value(
                np.zeros((1, 2), dtype=np.int8),
                {"x": Discrete(2)},  # type: ignore[arg-type]
            )

    def test_split_multidiscrete_discrete_only(self):
        d1 = Discrete(4)
        d2 = Discrete(7)
        merged = merge_spaces(d1, d2)
        flat = np.array([[2, 5]], dtype=np.int64)
        parts = split_multidiscrete_value(flat, {"a": d1, "b": d2})
        assert parts["a"].shape == (1,)
        assert parts["b"].shape == (1,)
        assert parts["a"][0] == 2 and parts["b"][0] == 5
        assert merged.contains(flat[0])

    def test_split_multidiscrete_mixed_with_multidiscrete(self):
        d1 = Discrete(3)
        md = MultiDiscrete([2, 4])
        d2 = Discrete(5)
        merged = merge_spaces(d1, md, d2)
        flat = np.array([[1, 0, 3, 2]], dtype=np.int64)
        parts = split_multidiscrete_value(flat, {"a": d1, "b": md, "c": d2})
        assert parts["a"].shape == (1,)
        assert parts["b"].shape == (1, 2)
        assert parts["c"].shape == (1,)
        np.testing.assert_array_equal(parts["b"][0], [0, 3])

    def test_split_multidiscrete_rejects_wrong_space_type(self):
        with pytest.raises(TypeError, match="Expected Discrete or MultiDiscrete"):
            split_multidiscrete_value(
                np.zeros((1, 2), dtype=np.int64),
                {"x": Box(0, 1, (2,), dtype=np.float32)},  # type: ignore[arg-type]
            )

    def test_split_value_routes_box(self):
        a = Box(0, 1, (2,), dtype=np.float32)
        b = Box(0, 1, (1,), dtype=np.float32)
        flat = np.array([[0.5, 0.25]], dtype=np.float32)
        parts = split_value(flat, {"a": a, "b": b})
        assert set(parts) == {"a", "b"}

    def test_split_value_routes_multibinary(self):
        flat = np.array([[1, 0, 1, 0]], dtype=np.int8)
        parts = split_value(flat, {"x": MultiBinary(2), "y": MultiBinary(2)})
        assert parts["x"].shape == (1, 2)

    def test_split_value_routes_discrete_family(self):
        flat = np.array([[1, 0, 2]], dtype=np.int64)
        parts = split_value(
            flat,
            {"a": Discrete(3), "b": MultiDiscrete([2, 3])},
        )
        assert parts["a"].shape == (1,)
        assert parts["b"].shape == (1, 2)

    def test_split_value_rejects_wrong_space_type(self):
        with pytest.raises(TypeError, match="Expected Box, MultiBinary"):
            split_value(
                np.zeros((1, 1), dtype=np.float32),
                {"t": gym.spaces.Text(max_length=4)},
            )


from .envs import (
    DictActionBoxEnv,
    DictActionDiscreteEnv,
    DictActionMultiBinaryEnv,
    DictActionMixedEnv,
    DictActionEmptyEnv,
    make_dict_action_env,
)


class TestVecMergeDictActionWrapper:
    """Test suite for the VecMergeDictActionWrapper."""

    def test_merge_dict_box_actions(self):
        """Test merging dictionary of Box action spaces."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        vec_env = DummyVecEnv([make_dict_action_env(DictActionBoxEnv, True)])
        wrapped_env = VecMergeDictActionWrapper(vec_env)

        # Check that action space is merged
        assert isinstance(wrapped_env.action_space, Box)
        assert wrapped_env.action_space.shape == (4,)  # 2 + 2

        # Test reset and step
        obs = wrapped_env.reset()
        assert obs.shape == (1, 4)

        # Test with flat action
        flat_action = np.array([[0.1, 0.2, 0.3, 0.4]], dtype=np.float32)
        obs, reward, done, info = wrapped_env.step(flat_action)
        assert obs.shape == (1, 4)
        assert reward.shape == (1,)
        assert done.shape == (1,)

        wrapped_env.close()

    def test_merge_dict_discrete_actions(self):
        """Test merging dictionary of Discrete action spaces."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        vec_env = DummyVecEnv([make_dict_action_env(DictActionDiscreteEnv, True)])
        wrapped_env = VecMergeDictActionWrapper(vec_env)

        # Check that action space is merged into MultiDiscrete
        assert isinstance(wrapped_env.action_space, MultiDiscrete)
        assert len(wrapped_env.action_space.nvec) == 2

        # Test reset and step
        obs = wrapped_env.reset()
        assert obs.shape == (1, 4)

        # Test with flat discrete action
        flat_action = np.array([[2, 1]], dtype=np.int64)
        obs, reward, done, info = wrapped_env.step(flat_action)
        assert obs.shape == (1, 4)

        wrapped_env.close()

    def test_merge_dict_multibinary_actions(self):
        """Test merging dictionary of MultiBinary action spaces."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        vec_env = DummyVecEnv([make_dict_action_env(DictActionMultiBinaryEnv, True)])
        wrapped_env = VecMergeDictActionWrapper(vec_env)

        # Check that action space is merged
        assert isinstance(wrapped_env.action_space, MultiBinary)
        assert wrapped_env.action_space.n == 5  # 3 + 2

        # Test reset and step
        obs = wrapped_env.reset()
        assert obs.shape == (1, 4)

        # Test with flat binary action
        flat_action = np.array([[1, 0, 1, 0, 1]], dtype=np.int8)
        obs, reward, done, info = wrapped_env.step(flat_action)
        assert obs.shape == (1, 4)

        wrapped_env.close()

    def test_empty_dict_action_raises(self):
        """Test that empty dictionary action space raises an assertion error."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        vec_env = DummyVecEnv([make_dict_action_env(DictActionEmptyEnv, True)])

        with pytest.raises(AssertionError, match="No Action Spaces to merge"):
            VecMergeDictActionWrapper(vec_env)

    def test_multiple_vec_envs_with_dict_actions(self):
        """Test wrapper with multiple parallel environments."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        # Create multiple environments
        n_envs = 4
        vec_env = DummyVecEnv(
            [make_dict_action_env(DictActionBoxEnv, True) for _ in range(n_envs)]
        )
        wrapped_env = VecMergeDictActionWrapper(vec_env)

        # Check action space
        assert isinstance(wrapped_env.action_space, Box)
        assert wrapped_env.action_space.shape == (4,)  # 2 + 2

        # Test reset with multiple envs
        obs = wrapped_env.reset()
        assert obs.shape == (n_envs, 4)

        # Test step with multiple envs
        actions = np.random.uniform(-1, 1, size=(n_envs, 4)).astype(np.float32)
        obs, rewards, dones, infos = wrapped_env.step(actions)
        assert obs.shape == (n_envs, 4)
        assert rewards.shape == (n_envs,)
        assert dones.shape == (n_envs,)
        assert len(infos) == n_envs

        wrapped_env.close()

    def test_step_async_and_wait_with_dict_actions(self):
        """Test async step functionality with dictionary actions."""
        from schola.sb3.utils import VecMergeDictActionWrapper

        vec_env = DummyVecEnv([make_dict_action_env(DictActionBoxEnv, True)])
        wrapped_env = VecMergeDictActionWrapper(vec_env)

        wrapped_env.reset()

        # Test step_async
        action = np.array([[0.1, 0.2, 0.3, 0.4]], dtype=np.float32)
        wrapped_env.step_async(action)

        # Test step_wait
        obs, rewards, dones, infos = wrapped_env.step_wait()
        assert obs.shape == (1, 4)
        assert rewards.shape == (1,)
        assert dones.shape == (1,)
        assert len(infos) == 1

        wrapped_env.close()


class TestRenderImagesWrapper:
    """Tests for RenderImagesWrapper"""

    def test_wrapper_fails_without_matplotlib(self):
        """Test that wrapper fails when matplotlib is not available"""
        mock_env = Mock()
        mock_env.num_envs = 1
        mock_env.observation_space = DictSpace(
            {"image": Box(low=0, high=1, shape=(3, 64, 64))}
        )

        with patch("schola.sb3.utils.plt", None):
            with pytest.raises(ImportError, match="matplotlib"):
                RenderImagesWrapper(mock_env)

    def test_convert_to_plt_format_grayscale(self):
        """Test convert_to_plt_format with grayscale image"""
        obs_space = Box(low=0, high=1, shape=(1, 64, 64))
        observation_space = DictSpace({"image": obs_space})

        mock_env = MagicMock()
        mock_env.num_envs = 1
        # Configure mock to return the real observation_space without interception
        mock_env.configure_mock(observation_space=observation_space)

        with patch("schola.sb3.utils.plt") as mock_plt:
            mock_plt.ion.return_value = None
            mock_plt.ioff.return_value = None
            mock_plt.show.return_value = None
            mock_axis = MagicMock()
            mock_axis.imshow.return_value = MagicMock()
            mock_plt.subplot.return_value = mock_axis

            wrapper = RenderImagesWrapper(mock_env)

            obs = np.random.rand(1, 64, 64)
            converted = wrapper.convert_to_plt_format(obs)

            # Should squeeze the channel dimension
            assert converted.shape == (64, 64)

    def test_convert_to_plt_format_rgb(self):
        """Test convert_to_plt_format with RGB image"""
        obs_space = Box(low=0, high=1, shape=(3, 64, 64))
        observation_space = DictSpace({"image": obs_space})

        mock_env = MagicMock()
        mock_env.num_envs = 1
        # Configure mock to return the real observation_space without interception
        mock_env.configure_mock(observation_space=observation_space)

        with patch("schola.sb3.utils.plt") as mock_plt:
            mock_plt.ion.return_value = None
            mock_plt.ioff.return_value = None
            mock_plt.show.return_value = None
            mock_axis = MagicMock()
            mock_axis.imshow.return_value = MagicMock()
            mock_plt.subplot.return_value = mock_axis

            wrapper = RenderImagesWrapper(mock_env)

            obs = np.random.rand(3, 64, 64)
            converted = wrapper.convert_to_plt_format(obs)

            # Should transpose to (H, W, C)
            assert converted.shape == (64, 64, 3)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
