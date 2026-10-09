# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Pytest fixtures for Minari data collection tests."""

import pytest
import gymnasium as gym
from typing import Callable
from concurrent import futures
import grpc
from Test.envs.imitation_server import GymToImitationServiceServicer
import schola.generated.ImitationConnector_pb2_grpc as imitation_connector_grpc


class SimplePolicy:

    def __init__(self, env: gym.Env):
        self.action_space = env.action_space
        self.action_space.seed(123)

    def __call__(self, observation):
        return self.action_space.sample()


@pytest.fixture(scope="function")
def simple_policy():
    return SimplePolicy


@pytest.fixture(scope="function")
def make_imitation_server():

    servers = []

    def _make_imitation_server(
        env_name: str | Callable[..., gym.Env],
        policy_class,
        wrappers: list = None,
        port: int = 0,
        options: dict = None,
    ):
        servicer = GymToImitationServiceServicer(env_name, policy_class, wrappers)
        server = grpc.server(futures.ThreadPoolExecutor(max_workers=1))
        imitation_connector_grpc.add_ImitationConnectorServiceServicer_to_server(
            servicer, server
        )
        port = server.add_insecure_port(f"[::]:{port}")
        server.start()
        servers.append(server)
        return port

    yield _make_imitation_server

    for server in servers:
        server.stop(0)

    for server in servers:
        server.wait_for_termination()


@pytest.fixture(
    scope="function",
    params=[("CartPole-v1", None), ("MountainCar-v0", None)],
    ids=lambda x: x[0],
)
def imitation_id_and_wrappers(request):
    env_id, wrappers = request.param
    return env_id, wrappers
