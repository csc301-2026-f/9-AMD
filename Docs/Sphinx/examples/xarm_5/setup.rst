Setup
=====

Adding the Robot
----------------

The 3D file of the X-Arm 5 provided by the manufacturer is a .STEP CAD file. To port this model into Unreal Engine, use the *Datasmith CAD Importer* plugin. Select the *Quickly add to the project* menu, and then *File Import* under Datasmith. Use the default options.

.. image:: /_static/examples/xarm_5_example/setup/datasmith-import-menu.png
   :alt: Datasmith import menu

It will be added to your open map, or a new one if none was open.

Select all actors under *xArm5_XF1300_*

.. image:: /_static/examples/xarm_5_example/setup/select-actors-2.png
   :alt: Select Actors

Convert selection to a Blueprint class

.. image:: /_static/examples/xarm_5_example/setup/make-blueprint-class.png
   :alt: Make new blueprint class

Select *Harvest Components*, and make the parent class a Pawn. Name it "X-Arm5"

.. image:: /_static/examples/xarm_5_example/setup/harvest-components.png
   :alt: Harvest Components

.. _assembly-components:

Assembly
~~~~~~~~

Open the newly created Pawn Blueprint for the XArm.

We are going to rename the static mesh components to make the imported names more clear. The imported meshes in the content drawer should be identical:

.. image:: /_static/examples/xarm_5_example/setup/staticmesh-content.png
   :alt: Static Meshed in Content

Follow the table below to rename each of the components we are going to use. Delete any static meshes not included here.

.. _assembly-table:

**Component Naming Table**

.. list-table::
   :header-rows: 1

   * - Static Mesh Component New Name
     - Old Name
   * - J1
     - EaEe1
   * - J2
     - EaEe1_3
   * - J3a
     - NONE_SAFE_5
   * - J3b
     - NONE_SAFE_7
   * - J4a
     - EaEe1_2
   * - J4b
     - EaEe2
   * - J5
     - NONE_SAFE
   * - J6
     - NONE_SAFE_4

For each scene group (J1_J1, J2_J2, ..., Tool_head_Tool_head) that contain the meshes, add corresponding *Physics Constraint* components named C1, C2, etc., **except** for C6.

Your component menu should look similar to the following now. Order does not matter.

.. image:: /_static/examples/xarm_5_example/setup/agent-components-named.png
   :alt: Components renamed

Configuration
~~~~~~~~~~~~~

Now, we need to setup the meshes & constraints to have the proper physics setup for the robot to behave correctly.

Meshes
^^^^^^

The meshes do not have a collision mesh by default when imported via Datasmith, so we need to configure these manually in order to use phyiscs on the robot. To do this, go to the location in the Content Drawer where the meshes were imported, and follow this table to configure the collisions.

You only need to change the collision on the static meshes we are using, reference these from the :ref:`assembly-components` section.

.. list-table::
   :header-rows: 1

   * - Static Mesh
     - Collision Type
     - Simple Collision Physical Material
     - Collision Presets
   * - EaEe1
     - 26DOP Simplified
     - NonSlippery
     - BlockAll
   * - All other 7
     - 26DOP Simplified
     - None
     - PhysicsActor

Your collsions should look like this for EaEe1:

.. image:: /_static/examples/xarm_5_example/setup/mesh-collision-J1.png
   :alt: EaEe1 mesh collision

and like this for the others:

.. image:: /_static/examples/xarm_5_example/setup/mesh-collision-others.png
   :alt: Other mesh collisions

*Note: If you don't see NonSlippery as an option, this is a part of the starter content - an option that can be selected when creating the uproject*

Head back to the X-Arm5 Blueprint, and setup the following options on each static mesh component:

1. For J1, set the following under *Constraints*

   .. image:: /_static/examples/xarm_5_example/setup/j1-mesh-constr.png
      :alt: J1 Mesh Constraints

2. For J6, set the following under *Constraints*

   .. image:: /_static/examples/xarm_5_example/setup/j6-mesh-constraints.png
      :alt: J6 Mesh Constraints

3. Set Linear & Angular damping to 5.0 for J1-6

4. We need to ensure the parts do not interfere with each other. To do so, we need to alternate the object type in the collision presets. Follow the table to configure this.

   - Each mesh should have the Collision Presets set to *Custom...* and the Collision Enabled set to *Collision Enabled (Query and Physics)*
   - On meshes that are of type *linkA*, set *linkA* to Block and *linkB* to Ignore under *Collision Responses*
   - Conversely, meshes of type *linkB* should have *linkA* set to Ignore and *linkB* to Block

   .. list-table::
      :header-rows: 1

      * - Static Mesh Component
        - Object Type
      * - J1
        - linkA
      * - J2
        - linkB
      * - J3a
        - linkA
      * - J3b
        - linkB
      * - J4a
        - linkA
      * - J4b
        - linkB
      * - J5
        - linkA
      * - J6
        - linkB

   The mesh collision presets menus should look like the following now:

   .. image:: /_static/examples/xarm_5_example/setup/collision_presets_A.png
      :alt: Collision presets for linkA

   .. image:: /_static/examples/xarm_5_example/setup/collision_presets_B.png
      :alt: Collision presets for linkB

5. Turn on *Simulate Physics* for J2-6. Leave it **off** for J1

6. Turn off *Enable Gravity* for J1-6

Constraints
^^^^^^^^^^^

Each piece of the arm needs to be connected to the next corresponding piece, as well as having its motion restricted to only the directions allowed by the physical robot.

1. For each C1-C5, set *Component Name 1* to the corresponding mesh, and *Component Name 2* to the next mesh (i.e. for C1, Component Name 1 is J1, and Component Name 2 is J2, etc.)

   - Be sure that C3a connects J3a and J3b, and likewise for C4a

2. Set the Linear Limits to *Locked* in each axis, for each physics constraint component

3. Set the Angular Limits as described in the table:

   .. list-table::
      :header-rows: 1

      * - Physics Constraint
        - Swing 1 Motion
        - Swing 2 Motion
        - Twist Motion
      * - C1
        - Free
        - Locked
        - Locked
      * - C2, C3b, C4b
        - Locked
        - Free
        - Locked
      * - C3a, C4a, C5
        - Locked
        - Locked
        - Locked

4. Set the *Angular Drive Mode* according to the table:

   .. list-table::
      :header-rows: 1

      * - Physics Constraint
        - Angular Drive Mode
      * - C1, C2, C3b, C4b
        - Twist and Swing
      * - C3a, C4a, C5
        - SLERP

   .. note::
      The Angular Drive Mode doesn't matter for constraints that have all angular limits locked. SLERP is just the default.

Functionality
~~~~~~~~~~~~~

Variables
^^^^^^^^^

Add the following variables:

- Links (Static Mesh Component Array)
- Constraints (Physics Constraint Component Array)
- LinkTransforms (Transform Array)

It should look like this now

.. image:: /_static/examples/xarm_5_example/setup/agent-variables.png
   :alt: Agent Variables

Functions
^^^^^^^^^

1. Set the function *EventGraph* as follows

		.. blueprint-file:: examples/xarm_5_example/xarm_agent/EventGraph.bp
				:heading: X-Arm5 > EventGraph
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-agent/agent-eventgraph.png
				:height: 400
				:zoom: -6

2. Create a function *ResetLocation*, and wire it as follows

		.. blueprint-file:: examples/xarm_5_example/xarm_agent/ResetLocation.bp
				:heading: X-Arm5 > ResetLocation
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-agent/agent-resetlocation.png
				:height: 400
				:zoom: -6

Creating the Target Blocks
---------------------------

- Inside Unreal Editor, go to tools -> New C++ Class
- Select Actor
- Keep the class type public, and title it "PickBlock"
- Code has been provided for PickBlock.h/.cpp. Paste it accordingly in the new files
- Compile the project.

Now, back in the editor, open the Content Drawer and locate the PickBlock C++ class that was created. Right click and select "Create Blueprint class based on PickBlock". Title it "BP_PickBlock" and place it where the X-Arm5 blueprint is stored. Next, we are going to create a custom material to provide color to the target blocks.

- Create a new material titled "M_Goal_Unlit"
- Open the material editor
- Change the Shading Model to Unlit
- In the material graph, add a Vector Parameter titled "Color". Set its default value to (1.0, 0.0, 1.0, 1.0) in RGBA.
- Add a Scalar Parameter titled "Intensity" and set its default value to 10.0.
- Then wire it all as so to complete the material

.. image:: /_static/examples/xarm_5_example/blueprint_images/material/material-wiring.png
    :alt: Material Graph

Now, back in BP_PickBlock, we need to set some parameters:

- Static Mesh is set to Shape_Sphere
- Under Materials, replace the default with M_Goal_Unlit
- Change the Collision Presets to match the following:

.. image:: /_static/examples/xarm_5_example/setup/pickblock-collision-config.png
	:alt: Collision Presets

- Scroll down to the Pick Block section
- Under Materials, set the Red, Yellow, and Blue materials to M_Goal_Unlit.

Creating the Environment
-------------------------

1. Create a new blueprint labeled "BP_XArmEnv"

   - When the popup window opens, drop down "All Classes" and search for "Gym"
   - Select the *Gym Connector Manager*

2. Go to Class Settings

   - Under Interfaces, go to Implemented Interfaces
   - Search for and add *Single Agent Schola Environment*
   - The interfaces should be populated with five items now.

As we did for PickBlock, we are going to need to add two new C++ files.

- Call them "XArmEnvComponent", keep them public and in the same location as the PickBlock source files.
- Paste in the provided code for XArmEnvComponent.h/.cpp
- Compile these changes, and reopen Unreal Engine
- In the components menu, add a new component by searching for XArm, select XArm Env. Call it "XArmEnvComp"

Add the following variables:

.. image:: /_static/examples/xarm_5_example/setup/xarm-env-variables.png
   :alt: Env Variables

Now, under interfaces, double click on SetEnvironmentOptions and SeedEnvironment. They should both show up in the EventGraph. Then, wire them both up as follows:

.. blueprint-file:: examples/xarm_5_example/xarm_env/EventGraph.bp
   :heading: BP_XArmEnv > EventGraph
   :imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-eventgraph.png
   :height: 400
   :zoom: -5

We have several functions to setup now. Follow the Blueprints to set them up

- ComputeReward

		.. blueprint-file:: examples/xarm_5_example/xarm_env/ComputeReward.bp
				:heading: BP_XArmEnv > ComputeReward
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-computereward.png
				:height: 400
				:zoom: -3

- ComputeStatus

		.. blueprint-file:: examples/xarm_5_example/xarm_env/ComputeStatus.bp
				:heading: BP_XArmEnv > ComputeStatus
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-computestatus.png
				:height: 400
				:zoom: -7

- Step

		.. blueprint-file:: examples/xarm_5_example/xarm_env/Step.bp
				:heading: BP_XArmEnv > Step
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-step.png
				:height: 400
				:zoom: -6

- Reset

		.. blueprint-file:: examples/xarm_5_example/xarm_env/Reset.bp
				:heading: BP_XArmEnv > Reset
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-reset.png
				:height: 400
				:zoom: -6

- InitializeEnvironment

		.. blueprint-file:: examples/xarm_5_example/xarm_env/InitializeEnvironment.bp
				:heading: BP_XArmEnv > InitializeEnvironment
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-initializeenvironment.png
				:height: 400
				:zoom: -7

- GetObservationSpace

		.. blueprint-file:: examples/xarm_5_example/xarm_env/GetObservationSpace.bp
				:heading: BP_XArmEnv > GetObservationSpace
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-getobservationspace.png
				:height: 400
				:zoom: -6

- CollectObservations

		.. blueprint-file:: examples/xarm_5_example/xarm_env/CollectObservations.bp
				:heading: BP_XArmEnv > CollectObservations
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-collectobservations.png
				:height: 400
				:zoom: -8

- GetActionSpace

		.. blueprint-file:: examples/xarm_5_example/xarm_env/GetActionSpace.bp
				:heading: BP_XArmEnv > GetActionSpace
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-getactionspace.png
				:height: 400
				:zoom: -4

- ApplyAction

		.. blueprint-file:: examples/xarm_5_example/xarm_env/ApplyAction.bp
				:heading: BP_XArmEnv > ApplyAction
				:imagefallback: /_static/examples/xarm_5_example/blueprint_images/xarm-env/xarm-env-applyation.png
				:height: 400
				:zoom: -6

World Setup
-----------

- Place the X-Arm5 in the world and label it "Agent1"
- BP_XArmEnv inside the world, with the actor for the environment placed at the base of the robot.
- In the world editor, select the BP_XArmEnv

  - Set Agent to Agent1

Your world should now look similar to the following on pressing the play button

.. image:: /_static/examples/xarm_5_example/setup/after-setup-run.png
   :alt: Initial stage
