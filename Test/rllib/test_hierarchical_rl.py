# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""
Hierarchical Reinforcement Learning Tests for Schola

This test suite validates that hierarchical RL environments work correctly with Schola.
Following RLlib's standard for hierarchical environments (see: https://docs.ray.io/en/latest/rllib/hierarchical-envs.html),
hierarchical RL is implemented as a special case of multi-agent RL.

The test environment (HierarchicalEnv) implements a two-level hierarchy:
- High-level agent: Issues goals/tasks at a coarse timescale (every N low-level steps)
- Low-level agents: Execute primitive actions to achieve the high-level goal

The environment follows PettingZoo's Parallel API for proper multi-agent support through Schola.

Test Coverage:
- Basic hierarchical environment functionality
- Integration with Schola's RayEnv wrapper
- Full episode execution with agent transitions
- Observation and reward structure validation
- Agent transition timing
- Space compatibility with RLlib
- Deterministic resets with seeds
"""

import pytest
import numpy as np
import gymnasium as gym
from gymnasium import spaces
from schola.rllib.env import RayEnv
from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
from schola.core.simulators.unreal.editor_simulator import UnrealEditor
from ray.rllib.env.multi_agent_env import MultiAgentEnv


class HierarchicalEnv:
    """
    A fake hierarchical environment following RLlib's multi-agent pattern for hierarchical RL.

    This implements a two-level hierarchy:
    - High-level agent: Issues goals/tasks at a coarse timescale (every N low-level steps)
    - Low-level agents: Execute primitive actions to achieve the high-level goal

    The environment follows the pattern described in:
    https://docs.ray.io/en/latest/rllib/hierarchical-envs.html

    This environment implements PettingZoo's Parallel API for proper multi-agent support.

    In this simple example:
    - High-level agent outputs a target position (goal)
    - Low-level agent tries to reach that position through movement actions
    - High-level agent acts every `high_level_interval` steps
    """

    metadata = {"render_modes": ["human"], "name": "HierarchicalEnv-v0"}

    def __init__(self, num_low_level_agents=2, high_level_interval=4, max_steps=20):
        """
        Args:
            num_low_level_agents: Number of low-level agents
            high_level_interval: How many low-level steps per high-level action
            max_steps: Maximum steps before episode ends
        """

        self.num_low_level_agents = num_low_level_agents
        self.high_level_interval = high_level_interval
        self.max_steps = max_steps

        # Define possible agents (PettingZoo API)
        self.possible_agents = ["high_level"]
        self.possible_agents.extend(
            [f"low_level_{i}" for i in range(num_low_level_agents)]
        )

        # High-level agent: outputs goal positions for low-level agents
        # Observation: current positions of all low-level agents
        # Action: target positions (goals) for each low-level agent
        self.high_level_obs_space = spaces.Box(
            low=-10.0,
            high=10.0,
            shape=(num_low_level_agents * 2,),  # x, y for each agent
            dtype=np.float32,
        )
        self.high_level_action_space = spaces.Box(
            low=-10.0,
            high=10.0,
            shape=(num_low_level_agents * 2,),  # target x, y for each agent
            dtype=np.float32,
        )

        # Low-level agents: try to reach goals set by high-level agent
        # Observation: own position + assigned goal
        # Action: movement direction
        self.low_level_obs_space = spaces.Box(
            low=-10.0,
            high=10.0,
            shape=(4,),  # own x, y, goal x, goal y
            dtype=np.float32,
        )
        self.low_level_action_space = spaces.Box(
            low=-1.0, high=1.0, shape=(2,), dtype=np.float32  # dx, dy movement
        )

        # State
        self.step_count = 0
        self.low_level_step_count = 0
        self.positions = None
        self.goals = None
        self.agents = []
        self.np_random = None

    # PettingZoo Parallel API methods
    def observation_space(self, agent):
        """Return the observation space for a specific agent."""
        if agent == "high_level":
            return self.high_level_obs_space
        else:
            return self.low_level_obs_space

    def action_space(self, agent):
        """Return the action space for a specific agent."""
        if agent == "high_level":
            return self.high_level_action_space
        else:
            return self.low_level_action_space

    def reset(self, seed=None, options=None):
        """Reset the environment (PettingZoo Parallel API)."""
        # Initialize random number generator
        if seed is not None:
            self.np_random = np.random.default_rng(seed)
        else:
            self.np_random = np.random.default_rng()

        # Initialize positions randomly
        self.positions = self.np_random.uniform(
            low=-5.0, high=5.0, size=(self.num_low_level_agents, 2)
        ).astype(np.float32)

        # Initialize goals (will be set by high-level agent)
        self.goals = np.zeros((self.num_low_level_agents, 2), dtype=np.float32)

        self.step_count = 0
        self.low_level_step_count = 0

        # Start with high-level agent active
        self.agents = ["high_level"]

        # Return observations for high-level agent
        obs = {"high_level": self._get_high_level_obs()}
        info = {"high_level": {}}

        return obs, info

    def step(self, action_dict):
        """
        Step the environment. Handles both high-level and low-level actions.
        """
        obs = {}
        rewards = {}
        terminateds = {}
        truncateds = {}
        infos = {}

        # Check if high-level agent is acting
        if "high_level" in action_dict:
            # High-level agent sets goals for low-level agents
            high_level_action = action_dict["high_level"]
            self.goals = high_level_action.reshape(self.num_low_level_agents, 2)

            # Now low-level agents will act
            self.agents = [f"low_level_{i}" for i in range(self.num_low_level_agents)]
            self.low_level_step_count = 0

            # Provide observations for low-level agents
            for i, agent_id in enumerate(self.agents):
                obs[agent_id] = self._get_low_level_obs(i)
                rewards[agent_id] = 0.0
                terminateds[agent_id] = False
                truncateds[agent_id] = False
                infos[agent_id] = {}

        else:
            # Low-level agents act
            for agent_id, action in action_dict.items():
                if agent_id.startswith("low_level"):
                    idx = int(agent_id.split("_")[-1])

                    # Update position based on action
                    self.positions[idx] += action * 0.5  # Scale movement
                    self.positions[idx] = np.clip(self.positions[idx], -10.0, 10.0)

                    # Calculate reward (negative distance to goal)
                    distance = np.linalg.norm(self.positions[idx] - self.goals[idx])
                    rewards[agent_id] = -distance

                    obs[agent_id] = self._get_low_level_obs(idx)
                    terminateds[agent_id] = False
                    truncateds[agent_id] = False
                    infos[agent_id] = {"distance_to_goal": str(distance)}

            self.low_level_step_count += 1

            # Check if it's time for high-level agent to act again
            if self.low_level_step_count >= self.high_level_interval:
                self.agents = ["high_level"]

                # High-level reward: average of low-level agents reaching goals
                avg_distance = np.mean(
                    [
                        np.linalg.norm(self.positions[i] - self.goals[i])
                        for i in range(self.num_low_level_agents)
                    ]
                )

                obs = {"high_level": self._get_high_level_obs()}
                rewards = {"high_level": -avg_distance}
                terminateds = {"high_level": False}
                truncateds = {"high_level": False}
                infos = {"high_level": {"avg_distance_to_goals": str(avg_distance)}}

                self.step_count += 1

        # Check episode termination
        done = self.step_count >= self.max_steps

        if done:
            for agent_id in self.agents:
                terminateds[agent_id] = True

        # Add __all__ key
        terminateds["__all__"] = all(terminateds.get(a, False) for a in self.agents)
        truncateds["__all__"] = False

        return obs, rewards, terminateds, truncateds, infos

    def _get_high_level_obs(self):
        """Get observation for high-level agent (all low-level positions)."""
        return self.positions.flatten()

    def _get_low_level_obs(self, agent_idx):
        """Get observation for low-level agent (own position + goal)."""
        return np.concatenate([self.positions[agent_idx], self.goals[agent_idx]])

    def render(self):
        """Render the environment (not implemented)."""
        pass

    def close(self):
        """Close the environment."""
        pass


def make_hierarchical_env(
    num_low_level_agents: int = 2, high_level_interval: int = 4, max_steps: int = 10
):
    """Factory function to create hierarchical environment (PettingZoo Parallel API)."""

    def _make():
        return HierarchicalEnv(
            num_low_level_agents=num_low_level_agents,
            high_level_interval=high_level_interval,
            max_steps=max_steps,
        )

    return _make


# Fixture for hierarchical environment tests
@pytest.fixture(scope="function")
def make_hierarchical_env_server(make_pettingzoo_env_server):
    """Create a hierarchical environment server using PettingZoo server."""

    def _factory(
        num_low_level_agents: int = 2, high_level_interval: int = 4, max_steps: int = 10
    ):
        return make_pettingzoo_env_server(
            make_hierarchical_env(num_low_level_agents, high_level_interval, max_steps)
        )

    return _factory


def test_hierarchical_env_basic():
    """Test that the hierarchical environment works in isolation."""
    env = HierarchicalEnv(num_low_level_agents=2, high_level_interval=3, max_steps=10)

    # Test reset
    obs, info = env.reset(seed=42)

    assert "high_level" in obs
    assert len(obs) == 1  # Only high-level agent initially
    assert env.observation_space("high_level").contains(
        obs["high_level"]
    ), f"High-level observation should be in space, got {obs['high_level']}"

    # Test high-level step
    high_level_action = {"high_level": env.action_space("high_level").sample()}
    obs, rewards, terminateds, truncateds, infos = env.step(high_level_action)

    # After high-level action, low-level agents should be active
    assert (
        "low_level_0" in obs
    ), f"Low-level agent 0 should be in observations on high-level step, got {obs}"
    assert (
        "low_level_1" in obs
    ), f"Low-level agent 1 should be in observations on high-level step, got {obs}"
    assert env.observation_space("low_level_0").contains(
        obs["low_level_0"]
    ), f"Low-level observation 0 should be in space, got {obs['low_level_0']}"
    assert env.observation_space("low_level_1").contains(
        obs["low_level_1"]
    ), f"Low-level observation 1 should be in space, got {obs['low_level_1']}"

    assert (
        len(obs) == 2
    ), f"Observations should have 2 agents on low-level step, got {obs}"
    assert (
        "high_level" not in obs
    ), f"High-level agent should not be in observations on low-level step, got {obs}"

    # Test low-level steps
    for _ in range(3):  # high_level_interval = 3
        low_level_actions = {
            "low_level_0": env.action_space("low_level_0").sample(),
            "low_level_1": env.action_space("low_level_1").sample(),
        }
        obs, rewards, terminateds, truncateds, infos = env.step(low_level_actions)

    # After 3 low-level steps, high-level agent should be active again
    assert "high_level" in obs
    assert env.observation_space("high_level").contains(
        obs["high_level"]
    ), f"High-level observation should be in space, got {obs['high_level']}"
    assert "low_level_0" not in obs
    assert len(obs) == 1

    env.close()


def test_hierarchical_env_with_schola(make_hierarchical_env_server):
    """Test hierarchical environment with Schola RayEnv wrapper."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    # Test that environment is created properly
    assert env.num_envs == 1, f"Number of environments should be 1, got {env.num_envs}"
    assert isinstance(
        env, MultiAgentEnv
    ), f"Environment should be a MultiAgentEnv, got {type(env)}"

    # Check possible agents
    assert set(env.possible_agents) == {
        "high_level",
        "low_level_0",
        "low_level_1",
    }, f"Possible agents should be ['high_level', 'low_level_0', 'low_level_1'], got {env.possible_agents}"

    # Test reset
    obs, info = env.reset(seed=42)

    assert isinstance(
        obs, dict
    ), f"Observations should be a dictionary, got {type(obs)}"
    assert isinstance(info, dict), f"Info should be a dictionary, got {type(info)}"
    assert "high_level" in obs, f"High-level agent should be in observations, got {obs}"
    assert env.get_observation_space("high_level").contains(
        obs["high_level"]
    ), f"High-level observation should be in space, got {obs['high_level']}"

    # Verify observation spaces
    for agent_id in env.possible_agents:
        assert (
            agent_id in env.single_observation_spaces
        ), f"Agent {agent_id} should be in single observation spaces, got {env.single_observation_spaces}"
        assert (
            agent_id in env.single_action_spaces
        ), f"Agent {agent_id} should be in single action spaces, got {env.single_action_spaces}"

    env.close()


def test_hierarchical_env_full_episode(make_hierarchical_env_server):
    """Test running a full episode with hierarchical environment through Schola."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    obs, info = env.reset(seed=42)

    high_level_steps = 0
    low_level_steps = 0
    done = False

    while not done:
        # Create actions based on active agents
        actions = {}

        if "high_level" in obs:
            # High-level agent acts
            high_level_steps += 1
            actions["high_level"] = env.get_action_space("high_level").sample()
        else:
            # Low-level agents act
            low_level_steps += 1
            for agent_id in obs.keys():
                if agent_id != "__all__":
                    actions[agent_id] = env.get_action_space(agent_id).sample()

        obs, rewards, terminateds, truncateds, infos = env.step(actions)

        # Check return format
        assert isinstance(
            obs, dict
        ), f"Observations should be a dictionary, got {type(obs)}"
        assert isinstance(
            rewards, dict
        ), f"Rewards should be a dictionary, got {type(rewards)}"
        assert isinstance(
            terminateds, dict
        ), f"Terminateds should be a dictionary, got {type(terminateds)}"
        assert isinstance(
            truncateds, dict
        ), f"Truncateds should be a dictionary, got {type(truncateds)}"
        assert isinstance(
            infos, dict
        ), f"Infos should be a dictionary, got {type(infos)}"

        # Check for __all__ key
        assert (
            "__all__" in terminateds
        ), f"__all__ should be in terminateds, got {terminateds}"
        assert (
            "__all__" in truncateds
        ), f"__all__ should be in truncateds, got {truncateds}"

        done = terminateds.get("__all__", False) or truncateds.get("__all__", False)

        # Safety check
        if high_level_steps + low_level_steps > 100:
            break

    # Verify hierarchical behavior: high-level steps should be much fewer than low-level
    assert high_level_steps > 0, "High-level agent should have acted"
    assert low_level_steps > 0, "Low-level agents should have acted"
    assert (
        low_level_steps >= high_level_steps
    ), "Low-level agents should step more times than high-level agent"

    env.close()


def test_hierarchical_env_observation_structure(make_hierarchical_env_server):
    """Test that hierarchical environment observations have correct structure."""
    env_server_port = make_hierarchical_env_server(
        num_low_level_agents=2, high_level_interval=4, max_steps=10
    )

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    obs, info = env.reset(seed=42)

    # High-level observation should contain info about all low-level agents
    assert "high_level" in obs
    high_level_obs = obs["high_level"]
    assert spaces.Box(
        low=-10.0, high=10.0, shape=(2 * 2,), dtype=np.float32  # x, y for each agent
    ).contains(
        high_level_obs
    ), f"High-level observation should be in space, got {high_level_obs}"

    # Step with high-level action to activate low-level agents
    high_level_action = {
        "high_level": spaces.Box(
            low=-10.0,
            high=10.0,
            shape=(2 * 2,),  # target x, y for each agent
            dtype=np.float32,
        ).sample()
    }
    obs, rewards, terminateds, truncateds, infos = env.step(high_level_action)

    # Low-level observations should contain own position + goal
    assert "low_level_0" in obs
    assert "low_level_1" in obs

    for agent_id in ["low_level_0", "low_level_1"]:
        low_level_obs = obs[agent_id]
        assert low_level_obs.shape == (4,)  # own x, y, goal x, goal y

    env.close()


def test_hierarchical_env_reward_structure(make_hierarchical_env_server):
    """Test that rewards are properly separated between hierarchy levels."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    obs, info = env.reset(seed=42)

    # High-level step
    high_level_action = {"high_level": env.single_action_spaces["high_level"].sample()}
    obs, rewards, terminateds, truncateds, infos = env.step(high_level_action)

    # Check rewards for low-level agents
    assert "low_level_0" in rewards
    assert "low_level_1" in rewards
    assert isinstance(rewards["low_level_0"], (int, float, np.integer, np.floating))
    assert isinstance(rewards["low_level_1"], (int, float, np.integer, np.floating))

    # Step low-level agents until high-level acts again
    for _ in range(4):  # high_level_interval = 4
        low_level_actions = {
            agent_id: env.single_action_spaces[agent_id].sample()
            for agent_id in obs.keys()
            if agent_id != "__all__"
        }
        obs, rewards, terminateds, truncateds, infos = env.step(low_level_actions)

    # Check high-level reward
    if "high_level" in rewards:
        assert isinstance(rewards["high_level"], (int, float, np.integer, np.floating))

    env.close()


def test_hierarchical_env_agent_transitions(make_hierarchical_env_server):
    """Test proper transitions between high-level and low-level agents."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    obs, info = env.reset(seed=42)

    # Initially, only high-level agent should be active
    assert "high_level" in obs
    assert "low_level_0" not in obs
    assert "low_level_1" not in obs

    # After high-level action, low-level agents should be active
    high_level_action = {"high_level": env.single_action_spaces["high_level"].sample()}
    obs, rewards, terminateds, truncateds, infos = env.step(high_level_action)

    assert "high_level" not in obs
    assert "low_level_0" in obs
    assert "low_level_1" in obs

    # After enough low-level steps, high-level should be active again
    for i in range(4):  # high_level_interval = 4
        low_level_actions = {
            agent_id: env.single_action_spaces[agent_id].sample()
            for agent_id in obs.keys()
            if agent_id != "__all__"
        }
        obs, rewards, terminateds, truncateds, infos = env.step(low_level_actions)

        if i < 3:  # Before the last step
            assert "low_level_0" in obs or "high_level" in obs
        else:  # Last step should transition to high-level
            assert "high_level" in obs or terminateds.get("__all__", False)

    env.close()


def test_hierarchical_env_spaces_compatibility(make_hierarchical_env_server):
    """Test that hierarchical environment spaces are compatible with RLlib."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    # Check that all agents have properly defined spaces
    for agent_id in env.possible_agents:
        obs_space = env.single_observation_spaces[agent_id]
        action_space = env.single_action_spaces[agent_id]

        assert (
            obs_space is not None
        ), f"Observation space for {agent_id} should not be None"
        assert (
            action_space is not None
        ), f"Action space for {agent_id} should not be None"
        assert isinstance(
            obs_space, spaces.Box
        ), f"Observation space for {agent_id} should be Box, got {type(obs_space)}"
        assert isinstance(
            action_space, spaces.Box
        ), f"Action space for {agent_id} should be Box, got {type(action_space)}"

        # Verify we can sample from spaces
        obs_sample = obs_space.sample()
        action_sample = action_space.sample()

        assert (
            obs_sample in obs_space
        ), f"Sample observation should be in space for {agent_id}"
        assert (
            action_sample in action_space
        ), f"Sample action should be in space for {agent_id}"

    env.close()


def test_hierarchical_env_deterministic_reset(make_hierarchical_env_server):
    """Test that hierarchical environment resets are reproducible with seeds."""
    env_server_port = make_hierarchical_env_server()

    simulator = UnrealEditor()
    protocol = GrpcProtocol(url="localhost", port=env_server_port)
    env = RayEnv(protocol, simulator)

    # Reset with same seed multiple times
    obs1, _ = env.reset(seed=42)
    obs2, _ = env.reset(seed=42)

    # Observations should be identical with same seed
    for agent_id in obs1.keys():
        assert agent_id in obs2
        assert np.allclose(
            obs1[agent_id], obs2[agent_id]
        ), f"Observations differ for agent {agent_id} with same seed"

    env.close()
