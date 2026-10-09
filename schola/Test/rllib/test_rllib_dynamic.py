# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Tests for ``RayEnv`` with PettingZoo parallel environments."""

import pytest
import numpy as np
from gymnasium import spaces
from typing import Dict, Any, Optional
from schola.rllib.env import RayEnv

# PettingZoo is required for these tests - fail immediately if not available
import pettingzoo
from pettingzoo.utils.env import ParallelEnv


class DynamicAgentEnv(ParallelEnv):
    """
    A dummy PettingZoo environment that tests dynamic agent scenarios.

    This environment simulates:
    - Agents terminating mid-episode (agent dies/completes objective)
    - Agents spawning mid-episode (new agent enters)
    - Agents truncating mid-episode (agent times out or leaves)

    The environment has a simple grid where agents can move around.
    Agents can be spawned or terminated based on specific conditions.
    """

    metadata = {
        "name": "dynamic_agent_v0",
        "render_modes": ["human"],
    }

    def __init__(
        self,
        max_agents: int = 5,
        initial_agents: int = 2,
        spawn_step: Optional[int] = 3,
        terminate_agent_at_step: Optional[int] = 5,
        truncate_agent_at_step: Optional[int] = 7,
        max_steps: int = 15,
    ):
        """
        Args:
            max_agents: Maximum number of agents that can exist
            initial_agents: Number of agents to start with
            spawn_step: Step at which to spawn a new agent (None to disable)
            terminate_agent_at_step: Step at which an agent terminates (None to disable)
            truncate_agent_at_step: Step at which an agent truncates (None to disable)
            max_steps: Maximum steps before episode ends
        """
        super().__init__()

        self.max_agents = max_agents
        self.initial_agents = initial_agents
        self.spawn_step = spawn_step
        self.terminate_agent_at_step = terminate_agent_at_step
        self.truncate_agent_at_step = truncate_agent_at_step
        self.max_steps = max_steps

        # All possible agents that could exist
        self.possible_agents = [f"agent_{i}" for i in range(max_agents)]

        # Observation: [x, y, health] - 2D position + health status
        self._obs_space = spaces.Box(low=0.0, high=10.0, shape=(3,), dtype=np.float32)

        # Action: [dx, dy] - movement in 2D
        self._action_space = spaces.Box(
            low=-1.0, high=1.0, shape=(2,), dtype=np.float32
        )

        # State tracking
        self.agents = []
        self.agent_positions = {}
        self.agent_health = {}
        self.step_count = 0
        self.next_spawn_idx = initial_agents
        self.spawned_mid_episode = False
        self.terminated_agents = set()
        self.truncated_agents = set()

    @property
    def num_agents(self):
        """Return the current number of active agents."""
        return len(self.agents)

    def observation_space(self, agent: str):
        """Return the observation space for a given agent."""
        return self._obs_space

    def action_space(self, agent: str):
        """Return the action space for a given agent."""
        return self._action_space

    def reset(self, seed: Optional[int] = None, options: Optional[Dict] = None):
        """Reset the environment to initial state."""
        if seed is not None:
            np.random.seed(seed)

        # Reset state
        self.step_count = 0
        self.next_spawn_idx = self.initial_agents
        self.spawned_mid_episode = False
        self.terminated_agents = set()
        self.truncated_agents = set()

        # Initialize agents
        self.agents = [f"agent_{i}" for i in range(self.initial_agents)]

        # Initialize positions and health
        self.agent_positions = {}
        self.agent_health = {}
        for agent in self.agents:
            self.agent_positions[agent] = np.random.uniform(0, 10, size=2)
            self.agent_health[agent] = 10.0

        # Return observations and infos
        observations = {}
        infos = {}
        for agent in self.agents:
            pos = self.agent_positions[agent]
            health = self.agent_health[agent]
            observations[agent] = np.array([pos[0], pos[1], health], dtype=np.float32)
            infos[agent] = {"health": str(health), "position": str(pos.tolist())}

        return observations, infos

    def step(self, actions: Dict[str, np.ndarray]):
        """
        Execute one step of the environment.

        Returns:
            observations: Dict of observations for active agents
            rewards: Dict of rewards for agents
            terminations: Dict indicating which agents terminated
            truncations: Dict indicating which agents truncated
            infos: Dict of additional information
        """
        self.step_count += 1

        observations = {}
        rewards = {}
        terminations = {}
        truncations = {}
        infos = {}

        # Process actions for current agents
        for agent in self.agents:
            if agent in actions:
                action = actions[agent]
                # Update position based on action
                self.agent_positions[agent] += action * 0.5
                # Clip to bounds
                self.agent_positions[agent] = np.clip(
                    self.agent_positions[agent], 0, 10
                )

                # Update health (simple decay)
                self.agent_health[agent] -= 0.1

        # Handle agent termination
        if (
            self.terminate_agent_at_step is not None
            and self.step_count == self.terminate_agent_at_step
            and len(self.agents) > 0
        ):
            # Terminate the first agent
            terminated_agent = self.agents[0]
            self.terminated_agents.add(terminated_agent)
            terminations[terminated_agent] = True
            self.agent_health[terminated_agent] = 0.0
            infos[terminated_agent] = {
                "termination_reason": "eliminated",
                "health": "0.0",
            }

        # Handle agent truncation
        if (
            self.truncate_agent_at_step is not None
            and self.step_count == self.truncate_agent_at_step
        ):
            # Truncate agent_1 if it exists, otherwise truncate the second agent in list
            if "agent_1" in self.agents:
                truncated_agent = "agent_1"
            elif len(self.agents) > 1:
                truncated_agent = self.agents[1]
            elif len(self.agents) > 0:
                truncated_agent = self.agents[0]
            else:
                truncated_agent = None

            if truncated_agent:
                self.truncated_agents.add(truncated_agent)
                truncations[truncated_agent] = True
                infos[truncated_agent] = {
                    "truncation_reason": "timeout",
                    "health": str(self.agent_health[truncated_agent]),
                }

        # Handle agent spawning
        if (
            self.spawn_step is not None
            and self.step_count == self.spawn_step
            and self.next_spawn_idx < self.max_agents
        ):
            # Spawn a new agent
            new_agent = f"agent_{self.next_spawn_idx}"
            self.agents.append(new_agent)
            self.agent_positions[new_agent] = np.random.uniform(0, 10, size=2)
            self.agent_health[new_agent] = 10.0
            self.next_spawn_idx += 1
            self.spawned_mid_episode = True
            infos[new_agent] = {"spawn_reason": "reinforcement", "health": "10.0"}

        # Build observations and rewards for ALL current agents (including those about to terminate/truncate)
        for agent in self.agents:
            pos = self.agent_positions[agent]
            health = self.agent_health[agent]
            observations[agent] = np.array([pos[0], pos[1], health], dtype=np.float32)

            # Simple reward: agent gets reward for surviving and moving
            reward = 0.1
            if agent in actions:
                reward += 0.05 * np.linalg.norm(actions[agent])
            rewards[agent] = reward

            # Default termination/truncation to False for active agents
            if agent not in terminations:
                terminations[agent] = False
            if agent not in truncations:
                truncations[agent] = False

            # Add info
            if agent not in infos:
                infos[agent] = {"health": str(health), "position": str(pos.tolist())}

        # Remove terminated and truncated agents from active list for NEXT step
        self.agents = [
            agent
            for agent in self.agents
            if agent not in self.terminated_agents
            and agent not in self.truncated_agents
        ]

        # Episode ends when max steps reached or no agents left
        episode_done = self.step_count >= self.max_steps or len(self.agents) == 0

        return observations, rewards, terminations, truncations, infos

    def close(self):
        """Clean up resources."""
        pass


def make_dynamic_env(**kwargs):
    """Factory function to create DynamicAgentEnv."""

    def _make():
        return DynamicAgentEnv(**kwargs)

    return _make


def test_agent_termination_mid_episode(make_pettingzoo_env_server):
    """Test that agents can terminate mid-episode."""
    # Create environment where agent terminates at step 5
    env_factory = make_dynamic_env(
        initial_agents=3,
        terminate_agent_at_step=5,
        spawn_step=None,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Should start with 3 agents
    assert (
        len(observations) == 3
    ), f"Expected 3 agents initially, got {len(observations)}"
    assert "agent_0" in observations
    assert "agent_1" in observations
    assert "agent_2" in observations

    # Step until termination
    for step in range(10):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        if step == 4:  # After step 5 (0-indexed)
            # agent_0 should have terminated
            assert (
                terminateds.get("agent_0", False) == True
            ), "agent_0 should be terminated"
            # Terminated agent still appears in observations for this final step
            assert (
                "agent_0" in observations
            ), "Terminated agent should appear in observations for final step"
            assert (
                len(observations) == 3
            ), "All agents including terminated one should be in observations"

            # Other agents should not be terminated
            assert terminateds.get("agent_1", False) == False
            assert terminateds.get("agent_2", False) == False

        if step > 4:
            # agent_0 should be absent in subsequent steps
            assert (
                "agent_0" not in observations
            ), "Terminated agent should not appear after termination step"
            assert (
                len(observations) == 2
            ), "Should have 2 remaining agents after termination"

    env.close()


def test_agent_spawning_mid_episode(make_pettingzoo_env_server):
    """Test that agents can spawn mid-episode."""
    # Create environment where agent spawns at step 3
    env_factory = make_dynamic_env(
        initial_agents=2,
        spawn_step=3,
        terminate_agent_at_step=None,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Should start with 2 agents
    assert (
        len(observations) == 2
    ), f"Expected 2 agents initially, got {len(observations)}"
    assert "agent_0" in observations
    assert "agent_1" in observations

    # Step until spawning
    for step in range(10):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        if step == 2:  # After step 3 (0-indexed)
            # agent_2 should have spawned
            assert "agent_2" in observations, "New agent should have spawned"
            assert len(observations) == 3, "Should have 3 agents after spawning"

            # Original agents should still be present
            assert "agent_0" in observations
            assert "agent_1" in observations

        if step > 2:
            # Spawned agent should remain present
            assert "agent_2" in observations, "Spawned agent should stay active"
            assert len(observations) == 3, "Should maintain 3 agents"

    env.close()


def test_agent_truncation_mid_episode(make_pettingzoo_env_server):
    """Test that agents can truncate mid-episode."""
    # Create environment where agent truncates at step 7
    env_factory = make_dynamic_env(
        initial_agents=3,
        truncate_agent_at_step=7,
        spawn_step=None,
        terminate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Should start with 3 agents
    assert (
        len(observations) == 3
    ), f"Expected 3 agents initially, got {len(observations)}"

    # Step until truncation
    for step in range(12):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        if step == 6:  # After step 7 (0-indexed)
            # agent_1 should have truncated
            assert (
                truncateds.get("agent_1", False) == True
            ), "agent_1 should be truncated"
            # Truncated agent still appears in observations for this final step
            assert (
                "agent_1" in observations
            ), "Truncated agent should appear in observations for final step"
            assert (
                len(observations) == 3
            ), "All agents including truncated one should be in observations"

            # Other agents should not be truncated
            assert truncateds.get("agent_0", False) == False
            assert truncateds.get("agent_2", False) == False

        if step > 6:
            # agent_1 should be absent in subsequent steps
            assert (
                "agent_1" not in observations
            ), "Truncated agent should not appear after truncation step"
            assert (
                len(observations) == 2
            ), "Should have 2 remaining agents after truncation"

    env.close()


def test_combined_dynamic_agents(make_pettingzoo_env_server):
    """Test combination of spawning, termination, and truncation."""
    # Create environment with all dynamic behaviors
    env_factory = make_dynamic_env(
        initial_agents=2,
        spawn_step=3,
        terminate_agent_at_step=6,
        truncate_agent_at_step=9,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Track agent count over time
    agent_counts = [len(observations)]

    # Step through episode
    for step in range(15):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        agent_counts.append(len(observations))

        # Check expected behaviors at specific steps
        if step == 2:  # After spawn at step 3
            assert len(observations) == 3, "Should have 3 agents after spawn"
            assert "agent_2" in observations, "Spawned agent should be present"

        if step == 5:  # After termination at step 6
            assert terminateds.get("agent_0", False) == True, "agent_0 should terminate"
            # Agent still in observations at termination step
            assert len(observations) == 3, "Should have 3 agents at termination step"

        if step == 6:  # Step after termination
            # Now agent_0 should be gone
            assert len(observations) == 2, "Should have 2 agents after termination"
            assert "agent_0" not in observations

        if step == 8:  # After truncation at step 9
            assert truncateds.get("agent_1", False) == True, "agent_1 should truncate"
            # Agent still in observations at truncation step
            assert len(observations) == 2, "Should have 2 agents at truncation step"
            assert "agent_2" in observations, "Spawned agent should be present"

        if step == 9:  # Step after truncation
            assert len(observations) == 1, "Should have 1 agent after truncation"
            assert "agent_2" in observations, "Only spawned agent should remain"

    # Verify agent count changed over time
    assert agent_counts[0] == 2, "Should start with 2 agents"
    assert max(agent_counts) == 3, "Should reach 3 agents at some point"
    # After all dynamics complete, should have 1 agent
    assert agent_counts[-1] == 1, "Should end with 1 agent"

    env.close()


def test_all_agents_terminate(make_pettingzoo_env_server):
    """Test edge case where all agents terminate mid-episode."""
    # Create environment where both agents terminate
    env_factory = make_dynamic_env(
        initial_agents=1,
        terminate_agent_at_step=3,
        spawn_step=None,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    assert len(observations) == 1, "Should start with 1 agent"

    # Step until all agents terminate
    for step in range(10):
        if len(observations) == 0:
            break

        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        if step == 2:  # After step 3
            # Agent should be terminated but still in observations for this step
            assert (
                len(observations) == 1
            ), "Terminated agent should still be in observations"
            assert (
                terminateds.get("agent_0", False) == True
            ), "agent_0 should be terminated"

        if step == 3:  # Step after termination
            # Now no agents should be present
            assert (
                len(observations) == 0
            ), "All agents should be gone after termination step"

    env.close()


def test_spawn_after_termination(make_pettingzoo_env_server):
    """Test spawning an agent after another has terminated."""
    # Create environment where one agent terminates, then another spawns
    env_factory = make_dynamic_env(
        initial_agents=2,
        terminate_agent_at_step=2,
        spawn_step=5,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Should start with 2 agents
    assert len(observations) == 2

    # Step through episode
    for step in range(10):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        if step == 1:  # After termination at step 2
            # Agent still in observations at termination step
            assert len(observations) == 2, "Should have 2 agents at termination step"
            assert terminateds.get("agent_0", False) == True

        if step == 2:  # Step after termination
            assert len(observations) == 1, "Should have 1 agent after termination"
            assert "agent_0" not in observations
            assert "agent_1" in observations

        if step == 4:  # After spawn at step 5
            assert len(observations) == 2, "Should have 2 agents after spawn"
            assert "agent_1" in observations, "Original agent should remain"
            assert "agent_2" in observations, "New agent should have spawned"
            assert "agent_0" not in observations, "Terminated agent should not reappear"

    env.close()


def test_dynamic_agent_action_spaces(make_pettingzoo_env_server):
    """Test that action spaces are correctly maintained for dynamic agents."""
    # Create environment with spawning
    env_factory = make_dynamic_env(
        initial_agents=1,
        spawn_step=2,
        terminate_agent_at_step=None,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Step and verify action spaces
    for step in range(5):
        # Check that all observed agents have valid action spaces
        for agent in observations.keys():
            assert (
                agent in env.single_action_spaces
            ), f"Agent {agent} should have an action space"
            action_space = env.single_action_spaces[agent]
            assert action_space is not None
            # Verify we can sample from it
            action = action_space.sample()
            assert action.shape == (2,), "Action should be 2D"

        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

    env.close()


def test_dynamic_agent_observation_spaces(make_pettingzoo_env_server):
    """Test that observation spaces are correctly maintained for dynamic agents."""
    # Create environment with spawning and termination
    env_factory = make_dynamic_env(
        initial_agents=2,
        spawn_step=3,
        terminate_agent_at_step=6,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Step and verify observations
    for step in range(10):
        # Check that all observations are valid
        for agent, obs in observations.items():
            assert (
                agent in env.single_observation_spaces
            ), f"Agent {agent} should have an observation space"
            obs_space = env.single_observation_spaces[agent]
            assert (
                obs in obs_space
            ), f"Observation {obs} should be in observation space for {agent}"
            assert obs.shape == (3,), "Observation should be 3D (x, y, health)"

        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

    env.close()


def test_dynamic_agent_rewards(make_pettingzoo_env_server):
    """Test that rewards are properly assigned to dynamic agents."""
    # Create environment with all dynamic behaviors
    env_factory = make_dynamic_env(
        initial_agents=2,
        spawn_step=3,
        terminate_agent_at_step=6,
        truncate_agent_at_step=None,
        max_steps=20,
    )

    # Start server
    port = make_pettingzoo_env_server(env_factory)

    # Create RayEnv
    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal.editor_simulator import UnrealEditor

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=port)
    env = RayEnv(protocol, simulator)

    # Reset environment
    observations, infos = env.reset()

    # Track rewards for each agent
    agent_rewards = {}

    # Step and verify rewards
    for step in range(10):
        actions = {
            agent: env.single_action_spaces[agent].sample()
            for agent in observations.keys()
        }
        observations, rewards, terminateds, truncateds, infos = env.step(actions)

        # Check that rewards are provided for all active agents
        for agent in observations.keys():
            if agent not in agent_rewards:
                agent_rewards[agent] = []
            # Rewards should be numeric
            assert agent in rewards, f"Agent {agent} should have a reward"
            assert isinstance(
                rewards[agent], (int, float, np.number)
            ), f"Reward for {agent} should be numeric"
            agent_rewards[agent].append(rewards[agent])

        # Check that terminated/truncated agents get final rewards
        for agent, terminated in terminateds.items():
            if terminated and agent != "__all__":
                assert (
                    agent in rewards
                ), f"Terminated agent {agent} should have a final reward"

    # Verify that spawned agent received rewards
    if "agent_2" in agent_rewards:
        assert (
            len(agent_rewards["agent_2"]) > 0
        ), "Spawned agent should have received rewards"

    env.close()
