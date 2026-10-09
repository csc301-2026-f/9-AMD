# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Tests for Minari ``DataCollector`` integration with Schola."""

import gymnasium as gym
import shutil
import os
import numpy as np

# Minari is required for these tests - fail immediately if not available
import minari
import pytest
from minari import DataCollector
from minari.utils import get_dataset_path
from schola.core.protocols.protobuf.offline_grpc_protocol import GrpcImitationProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from schola.minari.datacollector import ScholaDataCollector

_MINARI_CREATE_KWARGS = {
    "algorithm_name": "schola_test_policy",
    "author": "Schola CI",
    "author_email": "schola-ci@example.com",
    "code_permalink": "https://github.com/GPUOpen-LibrariesAndSDKs/Schola",
    "description": "Minari integration test dataset",
}


def wrap(env, wrappers):
    if not isinstance(env, gym.Env):
        env = env()
    if wrappers:
        for wrapper in wrappers:
            env = wrapper(env)
    return env


@pytest.fixture(scope="function")
def minari_dataset_dir(tmp_path, monkeypatch):
    dataset_path = tmp_path / "minari_datasets"
    dataset_path.mkdir()
    monkeypatch.setenv("MINARI_DATASETS_PATH", str(dataset_path))
    return dataset_path


def test_minari_integration(
    make_imitation_server, simple_policy, imitation_id_and_wrappers, minari_dataset_dir
):
    env_id, wrappers = imitation_id_and_wrappers

    port = make_imitation_server(env_id, simple_policy, wrappers)
    protocol = GrpcImitationProtocol(url="localhost", port=port)
    simulator = UnrealEditor()
    collector = ScholaDataCollector(protocol, simulator, seed=123)
    wrapped_env = wrap(gym.make(env_id), wrappers)
    policy_env = gym.make(env_id)
    policy = simple_policy(policy_env)

    for i in range(10):
        collector.step()

    minari_collector = DataCollector(wrapped_env)
    obs, info = minari_collector.reset(seed=123)
    for i in range(10):
        action = policy(obs)
        obs, reward, terminated, truncated, info = minari_collector.step(action)
        if terminated or truncated:
            obs, info = minari_collector.reset()

    create_kwargs = {**_MINARI_CREATE_KWARGS, "eval_env": env_id}
    minari_dataset = minari_collector.create_dataset(
        f"{env_id}-base-v0", **create_kwargs
    )
    schola_dataset = collector.create_dataset(f"{env_id}-schola-v0", **create_kwargs)

    assert (
        minari_dataset.total_steps == schola_dataset.total_steps
    ), f"Total steps do not match: {minari_dataset.total_steps} != {schola_dataset.total_steps}"

    # MinariDataset uses total_episodes, not num_episodes
    assert (
        minari_dataset.total_episodes == schola_dataset.total_episodes
    ), f"Number of episodes do not match: {minari_dataset.total_episodes} != {schola_dataset.total_episodes}"

    for episode, schola_episode in zip(
        minari_dataset.iterate_episodes(), schola_dataset.iterate_episodes()
    ):
        assert np.all(
            episode.actions == schola_episode.actions
        ), f"Actions do not match:"
        assert np.all(
            episode.observations == schola_episode.observations
        ), f"Observations do not match"
        assert np.all(
            episode.rewards == schola_episode.rewards
        ), f"Rewards do not match"
        assert np.all(
            episode.terminations == schola_episode.terminations
        ), f"Terminations do not match"
        assert np.all(
            episode.truncations == schola_episode.truncations
        ), f"Truncations do not match"
        assert episode.infos == schola_episode.infos, f"Infos do not match"

    collector.close()
    minari_collector.close()
