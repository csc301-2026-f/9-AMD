# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Tests for the offline gRPC imitation protocol and Minari export paths."""

import pytest
import numpy as np
import gymnasium as gym
from schola.core.protocols.protobuf.offline_grpc_protocol import GrpcImitationProtocol
import tempfile
import shutil
from pathlib import Path


@pytest.fixture(scope="function")
def imitation_protocol(make_imitation_server, simple_policy):
    """Create an imitation protocol connected to a CartPole server"""
    env_id = "CartPole-v1"
    port = make_imitation_server(env_id, simple_policy, wrappers=None)

    protocol = GrpcImitationProtocol(url="localhost", port=port)
    protocol.start()

    yield protocol

    protocol.close()


@pytest.fixture(scope="function")
def mountain_car_protocol(make_imitation_server, simple_policy):
    """Create an imitation protocol connected to a MountainCar server"""
    env_id = "MountainCar-v0"
    port = make_imitation_server(env_id, simple_policy, wrappers=None)

    protocol = GrpcImitationProtocol(url="localhost", port=port)
    protocol.start()

    yield protocol

    protocol.close()


def test_protocol_connection(imitation_protocol):
    """Test that the protocol can connect to the server"""
    assert imitation_protocol.channel_connected
    assert bool(imitation_protocol)


def test_get_definition(imitation_protocol):
    """Test that we can retrieve environment definitions"""
    # Send startup message first

    # get the definition
    definition = imitation_protocol.get_definition()

    # Verify the structure
    assert isinstance(definition, tuple)
    env_uids, agent_types, obs_spaces, action_spaces = definition

    # Check that we have environment definitions
    assert len(env_uids) > 0
    assert len(obs_spaces) > 0
    assert len(action_spaces) > 0

    # Verify CartPole spaces
    env_id = 0
    agent_id = "single_agent"
    assert env_id in obs_spaces
    assert agent_id in obs_spaces[env_id]
    assert agent_id in action_spaces[env_id]

    # CartPole observation space should be Box(4,)
    obs_space = obs_spaces[env_id][agent_id]
    assert isinstance(
        obs_space, gym.spaces.Box
    ), f"Observation space is not a Box: {obs_space}"
    assert obs_space.shape == (
        4,
    ), f"Observation space shape is not (4,): {obs_space.shape}"

    # CartPole action space should be Discrete(2)
    action_space = action_spaces[env_id][agent_id]
    assert (
        isinstance(action_space, gym.spaces.Discrete) and action_space.n == 2
    ), f"Action space is not a Discrete(2): {action_space}"


def test_get_initial_state(imitation_protocol):
    """Test that we can get initial state after startup"""

    imitation_protocol.get_definition()
    imitation_protocol.send_startup_msg(seeds=[42], options=[{}])

    # Get first data point - should include initial state
    data = imitation_protocol.get_data()

    # Unpack the data
    (
        observations,
        rewards,
        terminateds,
        truncateds,
        infos,
        initial_observations,
        initial_infos,
        actions,
    ) = data

    # Verify initial state is present on first call
    assert (
        len(initial_observations) > 0
    ), f"Initial observations is empty: {initial_observations}"
    assert (
        0 in initial_observations
    ), f"Initial observations does not contain env_id 0: {initial_observations}"
    assert (
        "single_agent" in initial_observations[0]
    ), f"Initial observations does not contain agent_id 'single_agent': {initial_observations[0]}"

    # Check initial observation shape
    initial_obs = initial_observations[0]["single_agent"]
    assert isinstance(initial_obs, np.ndarray)
    assert initial_obs.shape == (4,)  # CartPole observation space


def test_collect_trajectory(imitation_protocol):
    """Test collecting a full trajectory"""
    imitation_protocol.get_definition()
    imitation_protocol.send_startup_msg(seeds=[42], options=[{}])

    trajectory_observations = []
    trajectory_actions = []
    trajectory_rewards = []
    trajectory_terminated = []
    trajectory_truncated = []

    # Collect data until episode ends
    max_steps = 500  # Safety limit
    step = 0
    episode_ended = False

    while step < max_steps and not episode_ended:
        data = imitation_protocol.get_data()
        (
            observations,
            rewards,
            terminateds,
            truncateds,
            infos,
            initial_observations,
            initial_infos,
            actions,
        ) = data

        # Store trajectory data
        agent_id = "single_agent"
        env_id = 0

        if len(observations) > env_id and agent_id in observations[env_id]:
            trajectory_observations.append(observations[env_id][agent_id])
            trajectory_rewards.append(rewards[env_id][agent_id])
            trajectory_terminated.append(terminateds[env_id][agent_id])
            trajectory_truncated.append(truncateds[env_id][agent_id])

            if agent_id in actions[env_id]:
                trajectory_actions.append(actions[env_id][agent_id])

            # Check if episode ended
            if terminateds[env_id][agent_id] or truncateds[env_id][agent_id]:
                episode_ended = True

        step += 1

    # Verify we collected a trajectory
    assert len(trajectory_observations) > 0
    assert len(trajectory_rewards) > 0
    assert len(trajectory_actions) > 0

    # Verify observations are correct shape
    for obs in trajectory_observations:
        assert isinstance(obs, np.ndarray)
        assert obs.shape == (4,)

    # Verify episode ended properly
    assert episode_ended
    assert trajectory_terminated[-1] or trajectory_truncated[-1]


def test_multiple_episodes(imitation_protocol):
    """Test collecting multiple episodes"""
    imitation_protocol.get_definition()
    imitation_protocol.send_startup_msg(seeds=[42], options=[{}])

    num_episodes_to_collect = 3
    episodes_collected = 0
    max_total_steps = 1500  # Safety limit
    total_steps = 0

    while (
        episodes_collected < num_episodes_to_collect and total_steps < max_total_steps
    ):
        data = imitation_protocol.get_data()
        (
            observations,
            rewards,
            terminateds,
            truncateds,
            infos,
            initial_observations,
            initial_infos,
            actions,
        ) = data

        agent_id = "single_agent"
        env_id = 0

        if len(observations) > env_id and agent_id in observations[env_id]:
            # Check if episode ended
            if terminateds[env_id][agent_id] or truncateds[env_id][agent_id]:
                episodes_collected += 1

        total_steps += 1

    assert episodes_collected == num_episodes_to_collect


def test_actions_in_data(imitation_protocol):
    """Test that actions are included in the returned data"""
    imitation_protocol.get_definition()
    imitation_protocol.send_startup_msg(seeds=[42], options=[{}])

    # Get data
    data = imitation_protocol.get_data()
    (
        observations,
        rewards,
        terminateds,
        truncateds,
        infos,
        initial_observations,
        initial_infos,
        actions,
    ) = data

    agent_id = "single_agent"
    env_id = 0

    # Verify actions are present
    assert len(actions) > env_id
    assert agent_id in actions[env_id]

    # Verify action is valid (0 or 1 for CartPole)
    action = actions[env_id][agent_id]
    assert action in [0, 1]


def test_different_environments(mountain_car_protocol):
    """Test the protocol works with different environments"""
    # Get definition
    definition = mountain_car_protocol.get_definition()
    mountain_car_protocol.send_startup_msg(seeds=[42], options=[{}])
    env_uids, agent_types, obs_spaces, action_spaces = definition

    # Verify MountainCar spaces
    env_id = 0
    agent_id = "single_agent"

    # MountainCar observation space should be Box(2,)
    obs_space = obs_spaces[env_id][agent_id]
    assert isinstance(obs_space, gym.spaces.Box)
    assert obs_space.shape == (2,)

    # MountainCar action space should be Discrete(3)
    action_space = action_spaces[env_id][agent_id]
    assert isinstance(action_space, gym.spaces.Discrete)
    assert action_space.n == 3

    # Collect some data
    data = mountain_car_protocol.get_data()
    (
        observations,
        rewards,
        terminateds,
        truncateds,
        infos,
        initial_observations,
        initial_infos,
        actions,
    ) = data

    # Verify observations are correct shape
    obs = observations[env_id][agent_id]
    assert obs.shape == (2,)


def test_deterministic_seeding(make_imitation_server):
    """Test that seeding produces deterministic results"""
    env_id = "CartPole-v1"

    # Define a deterministic policy class
    class DeterministicPolicy:
        def __init__(self, env):
            pass

        def __call__(self, observation):
            return 1 if observation[0] > 0 else 0

    # Create two protocols with same seed
    port1 = make_imitation_server(env_id, DeterministicPolicy, wrappers=None)
    protocol1 = GrpcImitationProtocol(url="localhost", port=port1)
    protocol1.start()
    protocol1.get_definition()
    protocol1.send_startup_msg(seeds=[12345], options=[{}])

    port2 = make_imitation_server(env_id, DeterministicPolicy, wrappers=None)
    protocol2 = GrpcImitationProtocol(url="localhost", port=port2)
    protocol2.start()
    protocol2.get_definition()
    protocol2.send_startup_msg(seeds=[12345], options=[{}])

    # Collect first few observations from both
    for _ in range(5):
        data1 = protocol1.get_data()
        data2 = protocol2.get_data()

        obs1 = data1[0][0]["single_agent"]
        obs2 = data2[0][0]["single_agent"]

        # Observations should be identical with same seed
        np.testing.assert_array_almost_equal(obs1, obs2)

    protocol1.close()
    protocol2.close()
