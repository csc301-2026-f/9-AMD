Training an X-Arm 5 Robotic Arm with AMD Schola and Unreal Engine
=================================================================

This example provides a comprehensive walkthrough on how to setup all necessary pieces in order to perform training with the X-Arm 5 Robotic Arm.

Table of Contents
-----------------

.. toctree::
   :maxdepth: 2
   :caption: Setup and Training Tasks

   setup
   task1
   task2
   task3

Getting Started
---------------

Begin with the :doc:` setup` guide to configure your Unreal Engine project and set up the X-ARM 5 robot. This includes:

- Initial Schola and Unreal Engine setup
- Importing the robot model using Datasmith
- Configuring physics meshes and constraints
- Creating target blocks and environment blueprints
- Setting up the world

Training Tasks
--------------

After completing the setup, there are three example training tasks:

:doc:`task1`
   Learn the fundamentals by training the agent to reach and pick a specific colored block from fixed positions. This task introduces the basic observation, action, and reward structures.

:doc:`task2`
   Build upon Task 1 by introducing randomized block positions. The agent must generalize its learned behavior to handle varying spatial configurations.

:doc:`task3`
   The most challenging task combines random target colors with random positions. This task also introduces advanced reward mechanisms based on progress tracking.

Prerequisites
-------------

Before starting, please refer to the :doc:`../../guides/setup_schola` section to set up the Unreal Engine project and Schola plugin, if you haven't already.
