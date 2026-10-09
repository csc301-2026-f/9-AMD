# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

"""Helpers for driving OpenCppCoverage when pytest-cov is enabled (Windows / Unreal)."""

from __future__ import annotations

import logging
import os
import pathlib
import shutil
from dataclasses import dataclass, field
import subprocess
from typing import Any, List, Optional, Sequence

from unreal_test_classes import UnrealTestBatch

_logger = logging.getLogger(__name__)

SCHOLA_CPP_COVERAGE_ENV = "SCHOLA_MEASURE_CPP_COVERAGE"
OPENCPPCOVERAGE_ENV = "SCHOLA_OPENCPPCOVERAGE"


def is_pytest_cov_enabled(config: Any) -> bool:
    """Return True when pytest-cov is active and not disabled via --no-cov."""
    cov_plugin = config.pluginmanager.get_plugin("_cov")
    if cov_plugin is None:
        return False
    return not getattr(cov_plugin, "_disabled", False)


def find_opencppcoverage_exe() -> Optional[str]:
    override = os.environ.get(OPENCPPCOVERAGE_ENV)
    if override:
        p = pathlib.Path(override)
        if p.is_file():
            return str(p.resolve())
        w = shutil.which(override)
        if w:
            return w
    w = shutil.which("OpenCppCoverage")
    return w


def resolve_schola_plugin_root(uproject_path: pathlib.Path, cpp_hint: Optional[pathlib.Path]) -> Optional[pathlib.Path]:
    """Locate the Schola plugin folder (directory containing Schola.uplugin)."""
    plugins_schola = uproject_path.parent / "Plugins" / "Schola" / "Schola.uplugin"
    if plugins_schola.is_file():
        return plugins_schola.parent.resolve()
    root_uplugin = uproject_path.parent / "Schola.uplugin"
    if root_uplugin.is_file():
        return uproject_path.parent.resolve()
    if cpp_hint is not None:
        for parent in [cpp_hint.resolve()] + list(cpp_hint.resolve().parents):
            candidate = parent / "Schola.uplugin"
            if candidate.is_file():
                return candidate.parent.resolve()
    return None


@dataclass
class OpenCppCoverageRuntime:
    """Per-session OpenCppCoverage settings shared by ``UnrealCoverageTestBatch`` instances."""

    occ_exe: str
    plugin_root: pathlib.Path
    engine_path: Optional[pathlib.Path]

def build_opencppcoverage_argv(
    occ_exe: str,
    plugin_root: pathlib.Path,
    engine_path: Optional[pathlib.Path],
    editor_argv: Optional[List[str]] = None,
    *,
    input_coverages: Sequence[pathlib.Path] = (),
    export_binary: Optional[pathlib.Path] = None,
    export_cobertura: Optional[pathlib.Path] = None,
    export_html: Optional[pathlib.Path] = None,
) -> List[str]:
    """
    Build a full argv list: OpenCppCoverage ... -- <program> <args>.

    ``input_coverages`` may list multiple ``--input_coverage`` binary exports to merge.
    """
    pr = plugin_root.resolve()
    binaries = pr / "Binaries" / "Win64"
    sources = pr / "Source"

    argv: List[str] = [occ_exe]
    argv.append(f"--modules={binaries}")
    argv.append(f"--sources={sources}")

    for pat in (".gen.", "Intermediate", "ThirdParty", "\\Test\\", "*Test.cpp", "*.pb.h", "*.pb.cc"):
        argv.append(f"--excluded_sources={pat}")

    for ic in input_coverages:
        rp = ic.resolve()
        if rp.is_file():
            argv.append(f"--input_coverage={rp}")

    if engine_path is not None:
        eng = engine_path.resolve() / "Engine"
        if eng.is_dir():
            argv.append(f"--excluded_modules={eng}")

    if export_binary is not None:
        export_binary.parent.mkdir(parents=True, exist_ok=True)
        if export_binary.resolve().is_dir():
            raise ValueError(f"Export binary path {export_binary} is not a file")
        argv.append(f"--export_type=binary:{export_binary.resolve()}")

    if export_cobertura is not None:
        export_cobertura.parent.mkdir(parents=True, exist_ok=True)
        if export_cobertura.resolve().is_dir():
            raise ValueError(f"Export cobertura path {export_cobertura} is not a file")
        argv.append(f"--export_type=cobertura:{export_cobertura.resolve()}")
    
    if export_html is not None:
        # Guard must not be ``not export_html.is_dir()`` alone: absent paths are not
        # directories yet and must be created, not rejected.
        if export_html.exists() and not export_html.is_dir():
            raise ValueError(f"Export HTML path {export_html} is not a directory")
        export_html.mkdir(parents=True, exist_ok=True)
        argv.append(f"--export_type=html:{export_html.resolve()}")
    if editor_argv is not None:
        argv.append("--")
        argv.extend(editor_argv)
    return argv


def merge_opencpp_batches(occ: OpenCppCoverageRuntime, 
    coverage_batches: List[UnrealCoverageTestBatch], 
    export_path: pathlib.Path,
    export_html: bool = False,
    export_cobertura: bool = False,
) -> None:
    """
    Merge per-batch binary ``.cov`` files into HTML / Cobertura using OpenCppCoverage.

    Runs a no-op Windows process under OpenCppCoverage with only ``--input_coverage`` inputs
    and final ``--export_type`` flags (see OpenCppCoverage FAQ on merging).
    """
    input_coverages = [p.resolve() for batch in coverage_batches for p in batch.cov_report_files]
    input_coverages = [p for p in input_coverages if p.is_file() and p.exists()]

    if not input_coverages:
        _logger.warning("OpenCppCoverage merge skipped: no per-batch binary artifacts exist.")
        return

    merge_argv = build_opencppcoverage_argv(
        occ.occ_exe,
        occ.plugin_root,
        occ.engine_path,
        editor_argv=None,
        input_coverages=input_coverages,
        export_cobertura=export_path / "schola_cpp_coverage.xml" if export_cobertura else None,
        export_html=(export_path / "html") if export_html else None,
        export_binary=export_path / "schola_cpp_coverage.cov",
    )

    _logger.info("Merging %s OpenCppCoverage batch binary file(s) into final reports.", len(input_coverages))
    proc = subprocess.run(
        merge_argv,
        capture_output=True,
        text=True,
        timeout=int(os.getenv("SCHOLA_OPENCPPCOVERAGE_MERGE_TIMEOUT", "120")),
    )
    if proc.returncode != 0:
        _logger.warning(
            "OpenCppCoverage merge exited with code %s. stderr (truncated): %s",
            proc.returncode,
            (proc.stderr or "")[:2000],
        )
        return

def set_measure_cpp_coverage_env(enabled: bool) -> None:
    """Set env var read by Schola *.Build.cs when a coverage-friendly C++ build is needed."""
    if enabled:
        os.environ[SCHOLA_CPP_COVERAGE_ENV] = "1"
    else:
        os.environ.pop(SCHOLA_CPP_COVERAGE_ENV, None)

@dataclass
class UnrealCoverageTestBatch(UnrealTestBatch):
    """``UnrealTestBatch`` that launches the editor under OpenCppCoverage."""

    opencpp: OpenCppCoverageRuntime = field(kw_only=True)

    _cov_report_files: List[pathlib.Path] = field(default_factory=list)
    
    @property
    def current_cov_report_file(self) -> pathlib.Path:
        return self.report_path / f"cpp_coverage_{self.retries}.cov"

    @property
    def cov_report_files(self) -> List[pathlib.Path]:
        return self._cov_report_files

    def run(self, editor_path: pathlib.Path, uproject_path: pathlib.Path) -> subprocess.Popen:
        editor_args = [
            str(editor_path),
            f"-project={uproject_path}",
            "-ResumeRunTest",
            f"-CmdLineFile={self.command_line_args_file.resolve()}",
        ]
        argv = build_opencppcoverage_argv(
            self.opencpp.occ_exe,
            self.opencpp.plugin_root,
            self.opencpp.engine_path,
            editor_args,
            export_binary=self.current_cov_report_file,
        )
        print("Running Unreal Test Command: " + " ".join(argv))
        self._process = subprocess.Popen(
            argv,
            stdout=subprocess.PIPE,  # type: ignore
            stderr=subprocess.PIPE,  # type: ignore
            text=True,
        )
        # track every cov report file that we generate for later merging
        self._cov_report_files.append(self.current_cov_report_file)
        return self.process
