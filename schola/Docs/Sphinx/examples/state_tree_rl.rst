Training StateTree RL Agents via Hierarchical Reinforcement Learning
==============================================================================


This example reproduces the simple hierarchical Branch Selector environment (Evaluator + Forward/Backward Tasks). 
When the flag is true, the agent should move towards positive direction, and when the flag is false, the agent should move towards the negative direction.
 
.. image:: /_static/examples/state_tree_rl_example/overhead.gif
   :align: center
   :scale: 140
   :alt: Overhead view of the training environment

Prerequisites
-------------

Before starting, please refer to the :doc:`../../guides/setup_schola` guide to set up the Unreal Engine project and Schola plugin.


Architecture Overview
---------------------

Schola's StateTree integration consists of four main components:

1. :cpp:class:`StateTree Training Environment<AStateTreeTrainingEnvironment>` - Actor that manages the training loop and agent lifecycle
2. :cpp:class:`Step Inference Task<UStateTreeTask_StepInference>` - Task node that defines an agent's observation/action spaces
3. :cpp:class:`RL Decision Evaluator<UStateTreeEvaluator_RLDecision>` - Evaluator that drives branch selection via RL
4. :cpp:class:`RL Branch Condition<FStateTreeCondition_RLBranch>` - Condition that checks which branch the RL agent selected



Example: Hierarchical Basic Branch Selector
-------------------------------------------

This guide uses a simple hierarchical example where an evaluator learns to select forward or backward movement based on a flag value.

**StateTree Structure:**

.. code-block:: text

    Root State
    │
    ├── [Evaluator] BranchSelector
    │   └── Observes: current flag (0 or 1)
    │   └── Action Space: Discrete(2) → 0=Forward, 1=Backward
    │
    ├── → Forward State [RLBranch == 0]
    │      └── [Task] MoveForwardTask
    │          └── Observes: position
    │          └── Action Space: Box(-1, 1)
    │
    └── → Backward State [RLBranch == 1]
           └── [Task] MoveBackwardTask
               └── Observes: position
               └── Action Space: Box(-1, 1)


Setting Up a StateTree Training Environment
-------------------------------------------

Step 1: Create the StateTree Asset
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In Content Browser, right-click → **Artificial Intelligence** → **State Tree**
2. Open the StateTree editor
3. Design your state hierarchy with Forward and Backward states


Step 2: Create the Evaluator Blueprint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Create a Blueprint that inherits from :cpp:class:`UStateTreeEvaluator_RLDecision`
2. Override **Define** to set up the observation and action spaces

.. blueprint-file:: examples/statetree_example/ste_define.bp
   :heading: STEvaluator > Define
   :imagefallback: /_static/examples/state_tree_rl_example/ste_define.png
   :height: 350
   :zoom: -5


3. Override **Observe** to provide position observations

.. blueprint-file:: examples/statetree_example/ste_observe.bp
   :heading: STEvaluator > Observe
   :imagefallback: /_static/examples/state_tree_rl_example/ste_observe.png
   :height: 350
   :zoom: -5

4. Override **Compute Reward** to reward movement in the correct direction based on the task type

.. blueprint-file:: examples/statetree_example/ste_reward.bp
   :heading: STEvaluator > ComputeReward
   :imagefallback: /_static/examples/state_tree_rl_example/ste_reward.png
   :height: 350
   :zoom: -5

5. Override **ResetForEpisode** to reset the evaluator state at the beginning of each episode. In this example, we don't need to reset anything in the evaluator, so we only call observe to get the initial observation for the new episode.


    
Step 3: Create Task Blueprints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Create a shared Blueprint for Forward and Backward tasks inheriting from :cpp:class:`UStateTreeTask_StepInference`:
2. Add a boolean variable ``bIsForwardTask`` to differentiate behavior
3. Override **Define** to set up the observation and action spaces

.. blueprint-file:: examples/statetree_example/stt_define.bp
   :heading: STTask > Define
   :imagefallback: /_static/examples/state_tree_rl_example/stt_define.png
   :height: 350
   :zoom: -5

4. Override **Observe** to provide position observations

.. blueprint-file:: examples/statetree_example/stt_observe.bp
   :heading: STTask > Observe
   :imagefallback: /_static/examples/state_tree_rl_example/stt_observe.png
   :height: 350
   :zoom: -5

5. Override **Act** to apply movement based on the selected action

.. blueprint-file:: examples/statetree_example/stt_act.bp
   :heading: STTask > Act
   :imagefallback: /_static/examples/state_tree_rl_example/stt_act.png
   :height: 350
   :zoom: -5

6. Override **Compute Reward** to reward movement in the correct direction based on the task type

.. blueprint-file:: examples/statetree_example/stt_reward.bp
   :heading: STTask > ComputeReward
   :imagefallback: /_static/examples/state_tree_rl_example/stt_reward.png
   :height: 350
   :zoom: -5

7. Override **ResetForEpisode** to reset the task state at the beginning of each episode. In this example, we don't need to reset anything in the task, so we only call observe to get the initial observation for the new episode.


    
Step 4: Set Up Transitions with RLBranch Conditions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In the StateTree editor, create transitions from Root to Forward and Backward states
2. Add **RL Branch Check** condition to each transition
3. Set ``BranchIndex`` to 0 for Forward, 1 for Backward
4. Bind the condition's ``SelectedBranch`` input to your evaluator's output


Step 5: Create the Training Environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Create a Blueprint → Parent: :cpp:class:`StateTree Training Environment<AStateTreeTrainingEnvironment>`
2. Set the **StateTree Asset** reference
3. Add a boolean property ``bCurrentFlag``
4. Override **IsEpisodeOver** to define termination conditions

.. blueprint-file:: examples/statetree_example/statetree_environment_episode.bp
   :heading: StatetreeEnvironment > IsEpisodeOver
   :imagefallback: /_static/examples/state_tree_rl_example/is_ep_over.png
   :height: 350
   :zoom: -5

5. Override **OnEpisodeReset** to randomize the flag

.. blueprint-file:: examples/statetree_example/statetree_environment_reset.bp
   :heading: StatetreeEnvironment > OnEpisodeReset
   :imagefallback: /_static/examples/state_tree_rl_example/on_ep_reset.png
   :height: 350
   :zoom: -5


Step 6: Set Up the Level
^^^^^^^^^^^^^^^^^^^^^^^^^

1. Add your :cpp:class:`StateTree Training Environment<AStateTreeTrainingEnvironment>` actor to the level
2. Add the actor that will be controlled by the StateTree
3. Configure the environment's StateTree reference


Training Configuration
----------------------

Use RLlib via the Schola CLI to run training. Example command used for the simple hierarchical environment:

.. code-block:: bash

    schola rllib train ppo editor \
      --port 8002 \
      --save-final-policy \
      --export-onnx \
      --fcnet-hiddens 16 16 \
      --timesteps 100000

Argument explanations:

- ``--port 8002``: Port used by the Schola gRPC protocol (Unreal ↔ Python); change if you have port conflicts.
- ``--save-final-policy``: Persist the final learned policy to the checkpoint directory after training completes.
- ``--export-onnx``: Export the saved policy to ONNX for deployment in StateTree.
- ``--fcnet-hiddens 16 16``: Two hidden fully-connected layers with 16 units each for policy/value networks.
- ``--timesteps 100000``: Total environment timesteps for training.

Results
^^^^^^^

For the simple Direction Selector environment the trained policies produced deterministic behavior:

- The branch selector evaluator chooses ``0`` when the flag is true and ``1`` when the flag is false.
- The ``forward`` task policy consistently outputs action ``1``.
- The ``backward`` task policy consistently outputs action ``-1``.
