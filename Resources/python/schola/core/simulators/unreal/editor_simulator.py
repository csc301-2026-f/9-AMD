# Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""
Backward-compatible alias for :class:`ExternalSimulator`.

``UnrealEditor`` and ``ExternalSimulator`` represent the same concept: a
pre-started simulator that Schola connects to without managing its lifecycle.
New code should use ``ExternalSimulator`` directly.
"""

from schola.core.simulators.external_simulator import ExternalSimulator

UnrealEditor = ExternalSimulator
