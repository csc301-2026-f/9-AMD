CLI dataclass conventions
===========================

Schola training and utility CLIs use `cyclopts`_ with dataclasses. Follow these
rules when adding or renaming CLI-bound types in ``schola.scripts``.

.. _cyclopts: https://cyclopts.readthedocs.io/

Type suffixes
-------------

* **``*Config``** — Dataclass defines ``make()`` (and optional helpers such as
  ``make_n_async``) that construct a runtime object (simulator, gRPC protocol,
  etc.).

* **``*Settings``** — No ``make()``; holds only CLI-visible values
  (hyperparameters, paths, nested aggregates).

Root command dataclasses use the suffix **``ScriptSettings``** (e.g.
``Sb3ScriptSettings``, ``RllibScriptSettings``, ``MinariScriptSettings``).

Field names
-----------

Nested fields on ``*ScriptSettings`` and on aggregates such as
``EnvironmentSettings`` use **only** the pattern ``<area>_settings`` (snake_case
+ ``_settings``), **even when the annotated type is** ``*Config``. Example:
``simulator_settings: UnrealExecutableSimulatorConfig``,
``protocol_settings: GrpcProtocolConfig``.

Do not use ``*_config`` as an attribute name for CLI fields; **Config** vs
**Settings** is expressed in the **class name**, not the field name.

Renaming fields can change CLI flags; treat that as a breaking change unless
you add cyclopts aliases.

Cyclopts: ``Parameter(name="*")``
----------------------------------

* **``name="*"``** (with ``group=`` where applicable) is the **default** for
  user-facing nested dataclass fields on script commands. Inner options appear
  flat within the same help group, without an extra prefix from the Python
  field name.

* **Omit** ``name="*"`` only when you intentionally want nested / longer flag
  names; document the exception next to the field.

* **Meta commands** (e.g. ``command_template.py``) may use ``name="*"`` on
  synthetic parameters so a single token list parses before values are copied
  into ``hidden_script_args``.

* **Explicit** ``name="something"`` (not ``*``) is rare: stable CLI after Python
  renames, disambiguation, or shorter public tokens. Re-check ``--help`` and
  document.

Rule of thumb: ``group=`` selects the help section; ``name="*"`` merges inner
fields into that scope. Prefer ``name="*"`` + ``group=`` consistently across
SB3, RLlib, and Minari training entrypoints.

On the CLI this produces **single-level** kebab-case flags (for example
``--timesteps``, ``--save-final-policy``), not dotted paths such as
``--training-settings.timesteps``. Use ``schola … --help`` to confirm spellings
when copying examples from older posts or drafts.

Other
-----

* Shared hidden nested fields: ``IgnoreParameter = Parameter(show=False, parse=False)``
  in ``schola.scripts.common.settings`` (e.g. ``simulator_settings`` until a
  subcommand selects the simulator variant).

* Help text: use the string literal immediately after each field (cyclopts
  convention); keep long prose in class docstrings or Sphinx guides.

* Sub-apps: ``App(name="...", help="...")`` with kebab-case command names where
  applicable.
