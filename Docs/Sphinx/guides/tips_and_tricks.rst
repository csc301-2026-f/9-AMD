Tips and Tricks
===============

This page contains various tips and tricks for using Schola effectively, as well as common pitfalls and their solutions.

.. contents:: Table of Contents
    :local:

Training In The Editor Runs Slowly
----------------------------------

The most common cause of extremely slow training in the editor (e.g. less than 10 fps), is that by default the editor will reduce it's resource usage drastically when it isn't the active window. This can be changed in the settings. ``Edit`` → ``Editor Preferences`` → ``General`` → ``Performance`` → ``Use Less CPU when in Background``. Uncheck this option to allow the editor to use more resources when it is not the active window.


Getting More CPU/GPU Utilization
--------------------------------

If you find that training is running too slowly for your purposes, you can try building your environment into a standalone executable, and then running training from a separate python process.

.. note:: 
    
    If you are using a custom training script, you can instantiate your environment with a :py:class:`~schola.core.simulators.unreal.UnrealExecutable` simulator to achieve the same effect.

If your environment doesn't utilize camera sensors or other Unreal Engine features requiring rendering, you can also try running the training in headless mode. This can be done by setting the ``headless_mode`` parameter to ``True`` when instantiating your :py:class:`~schola.core.simulators.unreal.UnrealExecutable`, or by using the "--headless" flag with the CLI. When running in headless mode, the Unreal Engine will not render the environment, which can significantly increase training speed. 


Training Runs at a High FPS but My Agent Doesn't Train
------------------------------------------------------

When training at a very high FPS, actions that are scaled based on the framerate (e.g. Movement Input) may lead to the agent not training properly. This is caused by the agent's actions having very little impact on the game world, as they have all been scaled down, leading to a large number of very similar datapoints during training. 

To resolve this, you can either reduce the FPS of the training, or set a fixed simulation timestep for the Unreal Engine Process using the ``set_fps`` parameter when instantiating your :py:class:`~schola.core.simulators.unreal.UnrealExecutable`. As an example, if you set ``set_fps=30``, the simulation will run at 30 FPS, regardless of the actual FPS of the training, so each action will impact the game world exactly as if the game was running at 30fps.


Training is Complete but Training Script Hangs
----------------------------------------------

If you find that the training progress bar reports that training finished but the training script is hanging, it is likely that the post-training evaluation is still running. This can happen if you run with a very small number of training steps, and evaluation enabled (``--disable-eval`` is not set). If this is the case, due to deterministic decision making, the agent can get stuck in a loop or position where it never completes the environment.

To resolve this, you can either increase the number of training steps, or disable evaluation by setting ``--disable-eval`` when running your training script, or implement episode truncation logic in your environment's ``Step`` function to set ``bTruncated`` to ``true`` in the :cpp:struct:`FAgentState` after a fixed number of steps.

Understanding Algorithm Parameters and Options
----------------------------------------------

Schola supports multiple algorithms for training: PPO and SAC for Stable Baselines 3, as well as PPO, SAC, APPO, and Impala with Ray RLlib. Each has its own set of parameters and options. These are documented in a few places in Schola:

.. tabs::

    .. group-tab:: Python Dataclasses

        .. tabs:: 

            .. group-tab:: Stable Baselines 3
            
                * :py:class:`schola.scripts.sb3.settings.PPOSettings`
                * :py:class:`schola.scripts.sb3.settings.SACSettings` 

            .. group-tab:: Ray RLlib
                
                * :py:class:`schola.scripts.rllib.settings.PPOSettings`
                * :py:class:`schola.scripts.rllib.settings.SACSettings`
                * :py:class:`schola.scripts.rllib.settings.APPOSettings`
                * :py:class:`schola.scripts.rllib.settings.IMPALASettings`
    
    .. group-tab:: Unreal Engine Settings Structs

        .. tabs:: 

            .. group-tab:: Stable Baselines 3
                
                * :cpp:struct:`FSB3SACSettings`
                * :cpp:struct:`FSB3PPOSettings`

            .. group-tab:: Ray RLlib

                * :cpp:struct:`FRLlibPPOSettings`
                * :cpp:struct:`FRLlibSACSettings`
                * :cpp:struct:`FRLlibAPPOSettings`
                * :cpp:struct:`FRLlibIMPALASettings`

The order of precedence for default settings is Unreal Settings (only when launching the training process from Unreal), the CLI defaults, and then python dataclass default values. For example, the dataclass default values will be overridden by any values given a default in the CLI.

.. Note::

    While the settings and defaults are documented in Schola, the recommended source on what each settings does is the official documentation from Ray and Stable Baselines 3 as the settings are provided, almost entirely as is to the training algorithms provided by those platforms.

    * `Stable Baselines3 <https://stable-baselines3.readthedocs.io/en/master/index.html>`_.
    * `Ray RLlib <https://docs.ray.io/en/latest/rllib/index.html>`_.
