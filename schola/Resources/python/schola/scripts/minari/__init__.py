# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
"""Cyclopts CLI for Minari dataset collection with Schola (`schola minari`)."""

from cyclopts import App, Parameter, validators, group_extractors, Group

minari_app = App(name="schola-minari", help="Use Minari with Schola!")

from .collect import collect_app

minari_app.command(collect_app.meta, name="collect")

if __name__ == "__main__":
    minari_app()
