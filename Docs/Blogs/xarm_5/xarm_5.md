# Training X-ARM 5 robotic arm with AMD Schola and Unreal Engine

*By Noah Monti & Mehdi Saeedi.*

In this post, we will demonstrate how to train a robot arm with reinforcement learning in AMD Schola. In particular, we use the [X-ARM 5](https://www.ufactory.us/product/ufactory-xarm-5) robotic arm in Unreal® Engine with Schola v2.0, highlighting Schola's capabilities for reinforcement-learning based robotics.

## Robot arm

We used [UFACTORY US's X-Arm 5](https://www.ufactory.us/product/ufactory-xarm-5), a 5 DOF robotic arm. Please see the specifications of this robot and become familiar with it, as we will be replicating its functionality in the environments following.

## Structure of the environment & tasks

The simulated environment consists of the robotic arm, which we refer to as the *agent* in this post, and up to three target blocks: red, blue, and yellow. Our goal is to train the robot arm using reinforcement learning to accomplish different tasks that we will define.

- *Task 1:* Starting simple, the goal of this environment is to train the agent to reach a defined target block (red) at a fixed location, while learning to ignore the other two blocks.
- *Task 2:* This builds on Task 1 by randomizing the target block's location among the three fixed block locations.
- *Task 3:* Now, the target block is defined each episode, and the locations the blocks can spawn at are randomized within a defined area.

### Observations

For all tasks, we use the same observation space, which is a continuous space of floating-point numbers. It includes values representing the location of all blocks, the color of each block, the location of the end effector, and an additional specification on the color of the target block. This allows the agent to relate where it has moved the end effector to the location and color of the blocks, which, guided by the rewards, will push it over time to move towards the target directly.

### Actions

Again, the action space remains constant across all tasks. The agent outputs three floating-point values, which represent a force vector that is applied to the end effector of the robot to produce movement.

### Algorithm

To train the robot, we used the SAC algorithm based on [Stable-Baselines3](https://stable-baselines3.readthedocs.io/en/master/modules/sac.html). SAC, being an off-policy algorithm, allows the agent to reuse experiences from a replay buffer, learning efficiently from transitions collected under different policies throughout training. Additionally, SAC explicitly encourages exploration through entropy regularization, which rewards diverse actions and is useful for discovering successful strategies across our progressively complex tasks.

## Task 1 - Reach with fixed location

![Task 1](task1-image.png)

The agent is given rewards for reaching the correct target block, while penalties are given for picking an incorrect block, each step taken, and its distance from the target - among others. This serves to push the agent towards reaching the target as efficiently as possible, as it learns to minimize the steps taken and the optimal path to the target.

Once trained, the robot should exhibit behavior similar to the following:

<video controls width="640" muted playsinline>
  <source src="task1-long.mp4" type="video/mp4">
  Your browser does not support the video tag. Here's a
  <a href="task1-long.mp4">direct link to the video</a>.
</video>

## Task 2 - Reach with random location

![Task 2](task2-image.png)

In this task, we increase the complexity of the environment by allowing the target to appear on any of the three fixed block locations. While the positions of the blocks remain the same, their colors are randomly permuted in each episode. The agent must still identify and reach the red block, regardless of its position. The observation space, action space, and reward function remain unchanged from Task 1 - the only modification is the randomization of block colors across fixed locations.

After training, the robot should behave similar to the following:

<video controls width="640" muted playsinline>
  <source src="task2-long.mp4" type="video/mp4">
  Your browser does not support the video tag. Here's a
  <a href="task2-long.mp4">direct link to the video</a>.
</video>

## Task 3 - Random target, random location

![Task 3](task3-image.png)

This task represents a significant challenge for the agent. It now needs to identify the correct target, which is randomly selected for each episode, and can appear at any location, removing the fixed spawn positions used previously in Task 2. This adds both variability and complexity, requiring the model to adapt to changing conditions while still accurately selecting the target.

Finally, the trained robot should demonstrate behavior similar to the following:

<video controls width="640" muted playsinline>
  <source src="task3-long.mp4" type="video/mp4">
  Your browser does not support the video tag. Here's a
  <a href="task3-long.mp4">direct link to the video</a>.
</video>

## Conclusion

Through Task 1, Task 2, and Task 3, we progressively increased the complexity of the environment. Starting with a simple goal of reaching a fixed location, we introduced randomized targets and permuted block colors across fixed positions. Each step required the model to rely less on static cues and more on dynamic reasoning and perception. This tests the robustness of the learning algorithm and lays the groundwork for more advanced tasks in real-world scenarios where targets and conditions are rarely predictable.

As a follow-up, you can explore modifying the observation space or reward function to better guide the model's learning. For example, adding richer sensory input or shaping rewards to emphasize precision and efficiency. You could also try designing a new task altogether, such as requiring the agent to follow a sequence of targets.

## Running with AMD Schola

To learn how this robot arm was ported into Unreal Engine, including the setup process of the environment and the configurations for each task, please refer to the documentation in [X-ARM 5 Examples](LINK TO GPU OPEN).

