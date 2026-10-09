# Announcing Schola v2: Next-Generation Reinforcement Learning for Unreal Engine

By Alex Cann and Noah Monti 

We're excited to announce the release of **AMD Schola v2**, a major update to AMD's open-source reinforcement learning plugin for Unreal Engine 5. This release represents a significant leap forward in capabilities, performance, and ease of use for training and deploying AI agents in Unreal Engine environments.

## What is Schola?

Schola is a toolkit that bridges the gap between Unreal Engine and popular Python-based RL frameworks like Stable Baselines 3 and Ray RLlib. Whether you're training NPCs for games, developing robotic simulations, or exploring sim-to-real transfer, Schola provides the tools you need to create intelligent agents that learn from experience.

## What's New in AMD Schola v2?

### Flexible Inference Architecture with Agent/Policy/Stepper System

Schola v2 introduces a powerful and flexible architecture that decouples the inference process into components for maximum flexibility and reusability. This modular design allows you to mix and match different policies, stepping strategies, and agent implementations to suit your specific needs.

**Key Components:**

- **Agent Interface** - Define an agent that takes actions and makes observations
  - `UInferenceComponent` - Add inference to any actor
  - `AInferencePawn` - Standalone pawn-based agents
  - `AInferenceController` - AI controller pattern for complex behaviors

- **Policy Interface** - Plug in different inference backends, to turn observations into actions.
  - `UNNEPolicy` - Native ONNX inference with Unreal Engine's Neural Network Engine
  - `UBlueprintPolicy` - Custom Blueprint-based decision making
  - Extensible interface for custom policy implementations, and new inference providers

- **Stepper Objects** - Control inference execution patterns by coordinating agents and policies.
  - `SimpleStepper` - Synchronous, straightforward inference
  - `PipelinedStepper` - Overlap inference with simulation for better throughput
  - Build custom steppers for specialized performance requirements

This architecture means you can easily **switch between inference backends**, **optimize performance characteristics**, and **compose behaviors** without rewriting your agent logic. Whether you're prototyping with Blueprints or deploying production-ready neural networks, the same agent interface works seamlessly with your chosen policy and execution strategy.

### Minari Dataset Support

v2 introduces native support for the **Minari dataset format**, the standard for offline RL and imitation learning datasets. Minari provides a unified interface for storing and loading trajectory data, making it easier to share demonstrations and datasets across different projects and research communities.

### Dynamic Agent Management

One of the most powerful improvements in v2 is robust support for **agents being spawned and deleted mid-episode**. Previous versions required a static set of agents throughout an episode, or a predefined spawning function to spawn agents but v2 can now handle dynamic populations seamlessly.

This enables realistic scenarios like:

- **Battle Royale / Survival Games** - Agents can be eliminated and removed from training without breaking the episode
- **Population Simulations** - Spawn new agents based on game events or environmental triggers
- **Dynamic Team Composition** - Add or remove team members on the fly
- **Procedural Scenarios** - Dynamically create agents as players progress through procedurally generated content

The system lets you manage lifecycles the way you want, simply mark the agents as terminated when they die, or start reporting observations when they spawn. This makes it much easier to build realistic, dynamic environments that mirror actual game scenarios.

### Enhanced Command-Line Interface

Training from the command line is now more intuitive than ever:

```bash
# Stable Baselines 3
schola sb3 train ppo ...

# Ray RLlib
schola rllib train ppo ...

# Utilities
schola compile-proto
schola build-docs
```

The new CLI built with `cyclopts` provides better error messages, auto-completion support, and a more consistent interface across different RL frameworks.

### Blueprint Improvements

Working in Blueprints is smoother than ever:

- **Instanced Struct Based Objects** Full Blueprint Support for all points and spaces
- **Enhanced Blueprint Utilities** for spaces and points

### Updated Framework Support

AMD Schola v2 has been updated to support the latest versions of all major RL frameworks and libraries:

- **Gymnasium** - Full support for the latest Gymnasium API (1.1+)
- **Ray RLlib New API Stack** - Compatible with the latest Ray RLlib features and algorithms
- **Stable-Baselines3 2.x** - Updated to work with the newest SB3 release

These updates ensure you can leverage the latest features, bug fixes, and performance improvements from the RL ecosystem while training your agents in Unreal Engine.

## Getting Started with v2

### Prerequisites

- Unreal Engine 5.5+ (tested with 5.5-5.6)
- Python 3.10-3.12
- Visual Studio 2022 with MSVC v143 build tools (Windows)

### Installation

1. Clone or download AMD Schola v2 from the repository
2. Copy to your project's `/Plugins` folder
3. Install the Python package:

```bash
pip install -e <path to Schola>/Resources/python[all]
```

4. Enable the plugin in your Unreal project

## Compatibility

| Schola Version | Unreal Engine | Python | Status |
|----------------|---------------|--------|--------|
| 2.0.x          | 5.5-5.6       | 3.9-3.12 | ✅ Current |
| 1.3            | 5.5-5.6       | 3.9-3.11 | Legacy |
| 1.2            | 5.5           | 3.9-3.11 | Legacy |

## Community and Support

Schola is open source and we welcome contributions!

- **GitHub**: [GPUOpen-LibrariesAndSDKs/Schola](https://github.com/GPUOpen-LibrariesAndSDKs/Schola)
- **Documentation**: [gpuopen.com/manuals/schola](https://gpuopen.com/manuals/schola/schola-index/)
- **Issues & Discussions**: Submit on our GitHub repository

## Acknowledgments

AMD Schola v2 is the result of contributions from the AMD Software Technologies team, especially Tian Yue Liu, Mehdi Saeedi, and Nathan Hung, as well as the broader open-source community. Special thanks to all contributors who have helped make this release possible.

## Try It Today

AMD Schola v2 is available now under the MIT license. Whether you're building the next generation of game AI, training robots, or exploring cutting-edge research, Schola provides the tools you need to succeed.

Download Schola v2 and start building intelligent agents in Unreal Engine today!

---

*Schola is developed by AMD and released as part of the GPUOpen initiative. For more information about AMD's open-source tools and libraries, visit [gpuopen.com](https://gpuopen.com/).*

