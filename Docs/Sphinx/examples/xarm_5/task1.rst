Task 1 - Reach with Fixed Location
==================================
.. image:: /_static/examples/xarm_5_example/task1/task1.gif
   :alt: Task 1

We train the agent based on the following setup:

- **Observations** (simulated color sensor): Box (continuous) space of floating-point values with 24 dimensions filled as follows:

  - End Effector Location (3 Floats)
  - Target Color encoded as one-hot (3 Floats)
  - Block Position & One-hot color (6 Floats per block/18 total)

- **Actuator**: A force vector defined by a Box space with three dimensions. Each component of the vector is clamped from [–1,1], and the result is applied to the end effector of the robot.

These will remain constant throughout the tasks, the only changes we need to make are related to the spawning of the blocks and the rewards.

For the rewards, we will use the following structure for this task:

- +50 for correct target pick
- -10 for incorrect
- -0.01 for each step taken
- -5.0 for end effector out of bounds
- -0.01 multiplied based on distance from end effector to target.

The episode ends when agent picks any block, agent goes out of bounds, or the step limit for that episode is reached. Note that you can try different observations, actions, rewards and terminal conditions.

Next, select BP_XArmEnv within the world editor, then select XArmEnvComp, then set the following values within the *Reward* Section:

- Reward Pick Target: 50.0
- Reward Wrong Pick: -10.0
- Step Penalty: -0.01
- Terminate on Wrong Pick: True
- Auto Pick Distance: 25.0
- Use Out Of Bounds Penalty: True
- Out Of Bounds Distance: 125
- Out Of Bounds Penalty; -5.0
- Terminate on Out Of Bounds: True
- Use Simple Rewards: True
- Simple Distance Scale: -0.01
- Normalize Simple Distance: True

Set the following within the *Config* section:

- Block Anchor Locations:

  .. image:: /_static/examples/xarm_5_example/task1/block-anchor-locs.png
     :alt: Block anchor locations

- Spawn Blocks: True
- Num Blocks to Spawn: 3
- Block Class: BP_PickBlock
- Min Inter Block Distance: 50.0
- Max Spawn Distance from Base: 120.0
- Episode Length Steps: 500
- Block Uniform Scale: 0.25
- Use Goal Color Override: True
- Goal Color Override: Red

After pressing play, the environment should look similar to this:

.. image:: /_static/examples/xarm_5_example/task1/task1-setup.png
   :alt: Task 1 running

Finally, while the play in editor is running, Open a terminal emulator or command prompt, and run the following Python script::

.. code-block:: bash
   
   schola sb3 train sac editor --enable-checkpoints --checkpoint-dir .\ckpt\task1 --save-final-policy --port 8000 --timesteps 100000 --pbar
