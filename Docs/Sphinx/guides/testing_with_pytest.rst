Testing with pytest
====================

Schola uses `pytest` for Python unit tests and for **Unreal Engine automation tests** written in C++. Plugin hooks and collectors live under ``Source/conftest.py`` (with helpers in ``Source/unreal_test_classes.py`` and ``Source/opencppcoverage.py``).

Install test dependencies (from ``Resources/python``):

.. code-block:: bash

   pip install --group test -e ".[all]"

Python tests
------------

Python tests live in the top-level ``Test/`` directory. From the **plugin root** (the directory that contains ``pytest.ini``):

.. code-block:: bash

   python -m pytest Test --import-mode=importlib -n 0

The repository ``pytest.ini`` sets ``testpaths`` to ``Test`` and ``Source``, ``--basetemp=.pytest_tmp``, and (by default) ``-n 0`` so Unreal runs stay single-process friendly.

Unreal C++ automation tests
---------------------------

How it works
~~~~~~~~~~~~

* Test files are C++ sources named ``*Test.cpp`` (for example ``TeleportActuatorTest.cpp``).
* Each test is declared with Unreal's ``IMPLEMENT_SIMPLE_AUTOMATION_TEST`` macro. Pytest parses those macros and registers a pytest item per test.
* On **Windows only**, a custom test loop launches the **Unreal Editor** with ``-nullrhi``, ``-unattended``, and ``Automation RunTest`` for batched tests, then maps Unreal's JSON report back to pytest outcomes.
* On **Linux**, ``*Test.cpp`` files are not collected for this path (to avoid hanging or unsupported editor flows).

Host project and engine path
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pytest must find a ``.uproject`` for the game or sample project that includes Schola (for example ``YourGame/YourGame.uproject`` when the plugin is at ``YourGame/Plugins/Schola``). Resolution walks from the pytest config root and a few parent paths—see ``UnrealTestRunner.run_tests`` in ``Source/conftest.py``.

The Unreal installation is taken from:

* CLI: ``--engine-path``, or
* Config: ``engine_path`` in ``pytest.ini`` / ``pytest`` ``-o engine_path=...``

Build before tests
~~~~~~~~~~~~~~~~~~

Controlled by ``build_unreal`` in ``pytest.ini``, or ``--build-unreal`` / ``--no-build-unreal``. When enabled, Schola triggers an Unreal Build Tool (UBT) build for the host project before launching the editor.

Example (Schola under ``Plugins/Schola``, only C++ tests under ``Source``):

.. code-block:: bash

   python -m pytest ./Source --rootdir=./Source --import-mode=importlib -n 0 \
     -o engine_path="C:/Program Files/Epic Games/UE_5.7" -o build_unreal=true

Global Unreal batch timeout
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The total time allowed for all Unreal editor batches (seconds, default ``240``) comes from:

* CLI: ``--unreal-timeout SECONDS`` (overrides config), or
* Config: ``unreal_timeout`` in ``pytest.ini`` / ``pytest`` ``-o unreal_timeout=...``

C++ code coverage (OpenCppCoverage)
-----------------------------------

Overview
~~~~~~~~

On Windows, enabling Schola's coverage flags wraps the editor launch in `OpenCppCoverage <https://github.com/OpenCppCoverage/OpenCppCoverage>`__, scopes modules to the Schola plugin binaries and sources, excludes generated and third-party paths, merges per-batch ``.cov`` binaries, and optionally exports **HTML** and/or **Cobertura XML**.

This is **not** the same as ``pytest-cov``: ``pytest-cov`` covers Python code; Schola's C++ coverage is toggled only by the options below.

Prerequisites
~~~~~~~~~~~~~

1. Install OpenCppCoverage on the machine running tests.
2. Put ``OpenCppCoverage`` on ``PATH``, or set ``SCHOLA_OPENCPPCOVERAGE`` to the full path of the executable.

Enabling reports
~~~~~~~~~~~~~~~~

Use either CLI flags or ``pytest.ini``:

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - CLI
     - ``pytest.ini`` (boolean)
   * - ``--schola-cpp-coverage-html``
     - ``schola_cpp_coverage_html = true``
   * - ``--schola-cpp-coverage-cobertura``
     - ``schola_cpp_coverage_cobertura = true``

At least one of these must be true for OpenCppCoverage to run. If OpenCppCoverage is missing, tests still run but a warning is logged and no C++ coverage is produced.

Coverage-friendly C++ build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When **both** ``build_unreal`` is active **and** a Schola C++ coverage export is enabled, pytest sets ``SCHOLA_MEASURE_CPP_COVERAGE=1`` during the UBT build. Schola ``*.Build.cs`` files read this variable (for example they may set ``OptimizeCode = Never`` on editor targets) so line-level coverage is meaningful. If you skip the build step, ensure you have already built with this environment when you need accurate coverage.

Outputs
~~~~~~~

Under the session temp directory (typically ``.pytest_tmp/`` from ``pytest.ini``), next to the Unreal report folder (for example ``unreal_test_reports0``):

* **HTML:** ``html/`` (merged report)
* **Cobertura:** ``schola_cpp_coverage.xml``
* Per-batch binary artifacts may appear under ``batch_*`` before merge.

``SCHOLA_OPENCPPCOVERAGE_MERGE_TIMEOUT`` (default ``120``) limits how long the merge step waits, in seconds.

See also
--------

* Repository ``README.md`` ( **Build and Test → Testing** )
* ``pytest.ini`` for defaults
* ``Source/conftest.py``, ``Source/opencppcoverage.py``, ``Source/unreal_test_classes.py``
