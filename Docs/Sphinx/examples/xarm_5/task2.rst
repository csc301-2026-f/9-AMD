Task 2 - Reach with Random Location
===================================
.. image:: /_static/examples/xarm_5_example/task2/task2.gif
   :alt: Task 2

Go to the world editor, then to BP_XArmEnv, and under the *Config* section of XArmEnvComp, set *Randomize Color Permutation* to True.

By pressing play in the editor, you should now see the target blocks spawning in different positions, and this will change over each episode during training.

Try pressing play and stop multiple times to confirm each color is not spawning in the same slot every time.

Your environment should look alike these when running.

.. image:: /_static/examples/xarm_5_example/task2/task2-setup.png
   :alt: Task 2 running

.. image:: /_static/examples/xarm_5_example/task2/task2-setup-2.png
   :alt: Task 2 running alt

While the play in editor is running, launch schola with the following command. We are going to continue training from the model trained in Task 1::

.. code-block:: bash

   schola sb3 train sac editor --resume-from .\ckpt\task1\sac_final.zip --enable-checkpoints --checkpoint-dir .\ckpt\task2 --save-final-policy --port 8000 --timesteps 100000 --pbar
