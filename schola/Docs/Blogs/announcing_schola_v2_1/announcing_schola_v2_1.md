# Announcing Schola v2.1: State Trees, Scale, and a Richer Training Stack

By Alex Cann

We're excited to announce **AMD Schola v2.1**, a major follow-up to Schola v2. This release deepens integration with Unreal Engine workflows, adds new ways to train and evaluate at scale, and expands imitation learning data paths—while keeping the same core idea: bridge Unreal Engine to Python RL frameworks with a clear, customizable API.

If you are new to Schola, see our [v2 announcement](../announcing_schola_v2/announcing_schola_v2.md) for an overview of the new architecture introduced in schola v2.0.

## What's New in AMD Schola v2.1?

### State Tree Integration for Training and Inference

Schola v2.1 adds integration with the Unreal Engine **State Tree** framework so you can tie RL decisions into existing NPC logic during both **training and inference**. That makes it easier to blend learned policies with designer-authored logic, reuse familiar State Tree tooling, and keep high-level behavior structure visible in the editor.

### External Simulation and Kubernetes-Oriented Distributed Training

Training no longer has to assume a single local Unreal process for every workflow. v2.1 introduces an  **Kubernetes Support** for distributed training setups, so you can scale out learners and workers in cluster environments where that fits your pipeline. Pair this with the **distributed training** section in the [Schola documentation](https://gpuopen.com/manuals/schola/schola-index/) for architecture and setup notes.

### Stronger Imitation Learning and Minari Workflows

Imitation and offline RL get more first-class support: **Minari-related workflows** and collection tooling are expanded, with new tools for collecting demonstration data. Whether you are recording human or scripted play or curating datasets for behavior cloning, v2.1 tightens the loop between Unreal, the Python SDK, and minari.

### Eval Commands, YAML Configs, and a More Capable CLI

The command-line experience from v2 is enhanced in v2.1:

- **Eval commands** for both **Stable Baselines 3** and **Ray RLlib**, so you can evaluate checkpoints through the same `schola` entry points you use for training.
- **YAML configuration** for training CLIs, making experiments easier to reproduce and share without long one-liners.
- A new **project simulator** option that lets you build and train from a single command.
- Support for **spawning multiple processes** when training from the CLI, for workloads that benefit from parallel rollouts.

Together, these changes make day-to-day iteration and hand-off to teammates or automation simpler.

### Multi-Agent RLlib: Agent Type Mappings

Multi-agent training with RLlib is more explicit and flexible: v2.1 adds additional metadata so you can explicitly control how agents map to policies.

### Frame Stacking and Observation Shaping

For temporal observations, v2.1 adds **frame stacking** utilities—including **Box** and **Dict** stackers so you can stack frames consistently whether your observation is a single array or a collection of different observation types.

### ONNX Export and Inference Pipeline

ONNX export has been **updated and hardened**, with fixes for issues users hit when moving models from training into Unreal's inference path. If you rely on `UNNEPolicy` and exported models, upgrading should smooth that handoff.

### Python SDK: Async, Packaging, and Dependencies

Under the hood, the Python side sees meaningful improvements:

- **Async gRPC protocol support** (for SB3 environments) for running multiple Unreal Engine processes efficiently using non-blocking communication.
- Clearer **protocol and simulator** module layout after refactoring.
- **Modern packaging** via `pyproject.toml` instead of legacy `setup.py`-only flows.
- Bumped **gRPC** and **Protobuf toolchain** versions (`grpc` 1.80.0, `protoc` 31.1) and an updated **Ray RLlib** constraint band so you stay on supported combinations with the rest of the ecosystem.


## Upgrading from v2.0.x

If you already use Schola v2:

- Expect some **Python import path changes** if you depended on internal module names; align with the new `schola` package layout.
- **CLI subcommands** have been adjusted so that the simulator is now a subcommand of the train/eval command
- **Re-pin Python dependencies** after the gRPC, Protobuf, and RLlib updates.

See the [v2.1.0 release notes](../../Release%20Notes/v2_1_0.md) for a concise checklist-style summary.

## Getting Started with v2.1

### Prerequisites

- Unreal Engine 5.5+ (tested with 5.5–5.7)
- Python 3.10–3.12
- Visual Studio 2022 with MSVC v143+ build tools (Windows)

### Installation

1. Clone or download AMD Schola from the repository
2. Copy to your project's `/Plugins` folder
3. Install the Python package:

```bash
pip install -e <path to Schola>/Resources/python[all]
```

4. Enable the plugin in your Unreal project

## Compatibility

| Schola Version | Unreal Engine | Python   | Status   |
|----------------|---------------|----------|----------|
| 2.1.x          | 5.5–5.7       | 3.10–3.12 | Current |
| 2.0.x          | 5.5–5.6       | 3.10–3.12 | Legacy  |
| 1.3            | 5.5–5.6       | 3.9–3.11 | Legacy  |
| 1.2            | 5.5           | 3.9–3.11 | Legacy  |

## Community and Support

Schola is open source and we welcome contributions.

- **GitHub**: [GPUOpen-LibrariesAndSDKs/Schola](https://github.com/GPUOpen-LibrariesAndSDKs/Schola)
- **Documentation**: [gpuopen.com/manuals/schola](https://gpuopen.com/manuals/schola/schola-index/)
- **Issues & Discussions**: Submit on our GitHub repository

## Acknowledgments

AMD Schola v2.1 is the result of contributions from the AMD Software Technologies team, especially Tian Yue Liu, Mehdi Saeedi, and Noah Monti. Special thanks to all contributors who have helped make this release possible.

## Try It Today

AMD Schola v2.1 is available under the MIT license. Whether you are shipping game AI, running large-scale training, or combining learned policies with State Trees and handcrafted logic, this release is meant to meet you where you work—in the editor, in Python, and in the cluster.

Download Schola v2.1 and keep building intelligent agents in Unreal Engine.

---

*Schola is developed by AMD and released as part of the GPUOpen initiative. For more information about AMD's open-source tools and libraries, visit [gpuopen.com](https://gpuopen.com/).*
