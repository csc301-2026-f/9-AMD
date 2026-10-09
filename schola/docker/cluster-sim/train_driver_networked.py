#!/usr/bin/env python3
"""
Networked cluster training driver — runs on the Ray head node.

Unlike the sidecar variant, this driver:
  • Connects to a *remote* mock-UE container (ue-disc) for space discovery.
  • Uses configurable learner processes (``NUM_LEARNERS``, default 1).
  • Workers (``NUM_WORKERS``, default 2) connect to remote UE containers
    via a ``{worker_index}`` URL template (e.g. ``ue-{worker_index}``).
  • All gRPC traffic crosses the Docker network — no localhost.
"""

import os
import sys

import ray
from ray import tune
from ray.rllib.algorithms.ppo import PPO, PPOConfig
from ray.rllib.policy.policy import PolicySpec
from ray.rllib.connectors.env_to_module import FlattenObservations

from schola.rllib.env import RayVecEnv
from schola.rllib.env_runner import ScholaEnvRunner
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.external_simulator import ExternalSimulator

DISC_URL = os.environ.get("DISCOVERY_GRPC_URL", "ue-disc")
DISC_PORT = int(os.environ.get("DISCOVERY_GRPC_PORT", "50051"))
GRPC_PORT = int(os.environ.get("SCHOLA_GRPC_PORT", "50051"))
NUM_WORKERS = int(os.environ.get("NUM_WORKERS", "2"))
NUM_LEARNERS = int(os.environ.get("NUM_LEARNERS", "1"))
TIMESTEPS = int(os.environ.get("TRAIN_TIMESTEPS", "2000"))


def main():
    print("=" * 60)
    print("  Schola  —  Networked Docker Cluster Simulation")
    print("=" * 60, flush=True)

    # ── 1. Space discovery (remote UE container: ue-disc) ─────────────
    print(f"\n[head] Space discovery via REMOTE server at {DISC_URL}:{DISC_PORT}")
    protocol = GrpcProtocol(url=DISC_URL, port=DISC_PORT, credential_mode="insecure")
    simulator = ExternalSimulator()
    tmp_env = RayVecEnv(protocol, simulator)
    agent_names = {aid: aid for aid in tmp_env.possible_agents}
    print(f"[head] Discovered agents: {list(agent_names.keys())}")
    print(f"[head] Observation space: {tmp_env.single_observation_space}")
    print(f"[head] Action space:      {tmp_env.single_action_space}")
    tmp_env.close()

    # ── 2. Connect to Ray cluster ─────────────────────────────────────
    print("\n[head] Connecting to Ray cluster…")
    ray.init(address="auto")
    print(f"[head] Cluster resources: {ray.cluster_resources()}")
    nodes = ray.nodes()
    alive = [n for n in nodes if n["Alive"]]
    print(f"[head] Alive nodes: {len(alive)}")
    for n in alive:
        addr = n["NodeManagerAddress"]
        cpus = n["Resources"].get("CPU", 0)
        print(f"       -> {addr}  cpus={cpus}")

    # ── 3. Configure PPO ──────────────────────────────────────────────
    # URL template: ScholaEnvRunner expands {worker_index} on each
    # remote worker, so worker 1 → "ue-1", worker 2 → "ue-2", etc.
    print(
        f"\n[head] Configuring PPO  (learners={NUM_LEARNERS}, workers={NUM_WORKERS}, timesteps={TIMESTEPS})"
    )
    print(
        f"[head] URL template 'ue-{{worker_index}}' routes each worker to its UE container."
    )
    config = (
        PPOConfig()
        .api_stack(
            enable_rl_module_and_learner=True,
            enable_env_runner_and_connector_v2=True,
        )
        .environment(
            env_config={
                "protocol": GrpcProtocol,
                "protocol_args": {
                    "url": "ue-{worker_index}",
                    "port": GRPC_PORT,
                    "credential_mode": "insecure",
                    "environment_start_timeout": 120,
                },
                "port_offset_mode": "fixed",
                "simulator": ExternalSimulator,
                "simulator_args": {},
            },
        )
        .framework("torch")
        .env_runners(
            env_runner_cls=ScholaEnvRunner,
            num_env_runners=NUM_WORKERS,
            custom_resources_per_env_runner={"ue_worker": 1},
            env_to_module_connector=lambda env, spaces=None, device=None: FlattenObservations(
                input_observation_space=(
                    env.single_observation_space
                    if env is not None
                    else spaces["__env_single__"][0]
                ),
                input_action_space=(
                    env.single_action_space
                    if env is not None
                    else spaces["__env_single__"][1]
                ),
                multi_agent=True,
            ),
        )
        .multi_agent(
            policies={aid: PolicySpec() for aid in agent_names},
            policy_mapping_fn=lambda agent_id, *args, **kwargs: agent_id,
        )
        .training(
            lr=3e-4,
            train_batch_size=512,
            minibatch_size=128,
            num_epochs=3,
        )
        .learners(num_learners=NUM_LEARNERS, num_gpus_per_learner=0)
    )

    # ── 4. Train ──────────────────────────────────────────────────────
    print("\n[head] Launching training…\n")
    stop = {"num_env_steps_sampled_lifetime": TIMESTEPS}
    results = tune.run(
        PPO,
        config=config,
        stop=stop,
        verbose=2,
        storage_path="/tmp/ray_results",
    )

    # ── 5. Report ─────────────────────────────────────────────────────
    print("\n" + "=" * 60)
    print("  TRAINING COMPLETE (networked)")
    print("=" * 60)
    trial = results.trials[0]
    last = trial.last_result
    print(
        f"  Timesteps sampled ...... {last.get('num_env_steps_sampled_lifetime', '?')}"
    )
    mean_reward = last.get("env_runners", {}).get("episode_reward_mean", "?")
    print(f"  Mean episode reward .... {mean_reward}")
    print(f"  Trial path ............. {trial.path}")
    print("=" * 60, flush=True)

    ray.shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())
