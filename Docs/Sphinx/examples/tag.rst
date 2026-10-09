Training NPCs to Play a MultiAgent Game of Tag
==============================================

In this tutorial, we will create a multi-agent environment where the agents are trained to play a 3v1 game of tag. Specifically, we create one runner agent which tries to avoid being caught and three tagger agents with the goal of catching the runner. The agents can move forward, left and right and can sense both their surrounding objects, as well as the locations of other agents.

.. image:: /_static/examples/tag_example/Tag.gif
   :align: center  
   :scale: 150

The Structure of the Environment in Unreal Engine
-------------------------------------------------

To build the game (called environment hereafter), we need to create the following in our Unreal Engine project:  

-  :localref:`Direction and Distance Observer<Creating the Custom Direction and Distance Observer>`: A :cpp:class:`UActorComponent` that implements the Schola Sensor interface, allowing the taggers to observe the direction and distance of other agents.
-  :localref:`Agent blueprint<Creating the Agents>`: A subclass of `Character <https://dev.epicgames.com/documentation/en-us/unreal-engine/characters-in-unreal-engine>`_, which includes the shape and appearance of the agent.
-  :localref:`Environment definition<Creating the Environment Definition>`: A subclass of :cpp:class:`GymConnectorManager <AGymConnectorManager>` that implements the :cpp:class:`Multi-Agent Schola Environment<IMultiAgentScholaEnvironment>`. It includes the logic of :cpp:func:`InitializeEnvironment<IMultiAgentScholaEnvironment::InitializeEnvironment>` the environment before training starts, :cpp:func:`Reset<IMultiAgentScholaEnvironment::Reset>` the environment between different episodes of training, and :cpp:func:`Step<IMultiAgentScholaEnvironment::Step>`, the environment during an episode.  
-  :localref:`Map<Creating the Map>`: The game map includes the floor, four walls, agents, and the environment.  

Initial Setup  
-------------
Please refer to the guide on :doc:`../guides/setup_schola` to set up the Unreal Engine project and Schola plugin.  

Creating the Custom Direction and Distance Observer
---------------------------------------------------

There are a variety of built-in observer classes available in Schola, such as the :cpp:class:`RotationObserver<URotationObserver>` and :cpp:class:`RayCastSensor<URayCastSensor>`. Custom observers are needed when we need specific observations not covered by the built-in observers. In this example, we will create a custom observer (inheriting from :cpp:class:`UActorComponent`) that implements methods defined in the :cpp:class:`IScholaSensor` interface. The function is to allow taggers to observe the direction and distance of other agents relative to the current agent in the game. It will return the distance normalized by the environment size and the direction as a unit vector. The :cpp:func:`~IScholaSensor::GetObservationSpace` function will return the observation space, and the :cpp:func:`~IScholaSensor::CollectObservations` function will collect and return the observations.

#. Create a new Blueprint Class with parent class :cpp:class:`ActorComponent<UActorComponent>`, and name it ``DirectionDistanceObserver``.  
#. Under ``Class Settings`` → ``Interfaces`` add ``ScholaSensor``. Three function should appear under ``Interfaces``.
#. Add a new integer variable. Name it ``EnvSize``, and set the default value to 5000. This stores the maximum possible distance between two agents within the environment.
#. Add a new `Actor <https://dev.epicgames.com/documentation/en-us/unreal-engine/actors-in-unreal-engine>`_ variable. Name it ``Target``. This stores the target agent that the observer will track.
#. Set the :cpp:func:`~IScholaSensor::GetObservationSpace` and :cpp:func:`~IScholaSensor::CollectObservations` blueprints as shown below. 

.. blueprint-file:: examples/tag_example/tag_direction_distance_observer/CollectObservations.bp
   :heading: DirectionDistanceObserver > CollectObservations
   :imagefallback: /_static/examples/tag_example/tag_direction_distance_observer/CollectObservations.png
   :height: 350
   :zoom: -5

.. blueprint-file:: examples/tag_example/tag_direction_distance_observer/GetObservationSpace.bp
   :heading: DirectionDistanceObserver > GetObservationSpace
   :imagefallback: /_static/examples/tag_example/tag_direction_distance_observer/GetObservationSpace.png
   :height: 300
   :zoom: -4

Creating the Agents
-------------------

Creating the Tagger Class
~~~~~~~~~~~~~~~~~~~~~~~~~

#. Create a new Blueprint Class with parent class `Character <https://dev.epicgames.com/documentation/en-us/unreal-engine/characters-in-unreal-engine>`_, and name it ``Tagger``.
#. Add any desired `static meshes <https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/StaticMesh>`_ and `material <https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-materials>`_ as the agent’s body. 
#. Set ``Details`` → ``Character Movement: Walking`` → ``Max Walk Speed`` to 520 cm/s.
#. Set ``Details`` → ``Character Movement (Rotation Settings)`` → ``Orient Rotation to Movement`` to true. This allows the agent to rotate using the :cpp:class:`Movement Input Actuator<UMovementInputActuator>`.
#. Set ``Details`` → ``Pawn`` → ``Use Controller Rotation Yaw`` to false. This allows the agent to rotate using the :cpp:class:`Movement Input Actuator<UMovementInputActuator>`.
#. In ``Details`` → ``Tags``, add a new tag, and set the value to ``Tagger``. This tag is used by the :cpp:class:`RayCastSensor<URayCastSensor>` to detect different objects.
#. Add a new pawn variable ``Target`` (Object Reference) and make it publicly editable (by clicking on the eye icon to toggle the visibility)
#. Add a new boolean variable ``Hit Wall`` to store whether the tagger agent has hit a wall in the current step
#. Add a new boolean variable ``Caught Target`` which tracks whether the tagger agent has caught the runner agent in the current step.

.. image:: /_static/examples/tag_example/taggerSettings.png

Attaching the Ray Cast Observer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#. Add a :cpp:class:`RayCastSensor<URayCastSensor>` component.   
#. Set ``Details`` → ``Sensor properties`` → :cpp:var:`~URayCastSensor::NumRays` to 36.  
#. Set ``Details`` → ``Sensor properties`` → :cpp:var:`~URayCastSensor::RayDegrees` to 360.  
#. Set ``Details`` → ``Sensor properties`` → :cpp:var:`~URayCastSensor::RayLength` to 2048.  
#. In ``Details`` → ``Sensor properties`` → :cpp:var:`~URayCastSensor::TrackedTags`, add two new elements and set the tags to ``Runner`` and ``Tagger``.  

.. .. note::
..
..    For more information on attaching actuators and observers, please refer to the :ref:`Attaching Actuators and Observers Section of Example 2 <attaching-actuators-and-observers>`.
..    Commented out because the reference material is outdated (as of 2025-11-12)

Attaching the Movement Input Actuator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We will use two :cpp:class:`Movement Input Actuators <UMovementInputActuator>` to move the agent. One lateral axis actuator to steer, and one forward axis actuator to move the agent forward.

#. Add an :cpp:class:`Movement Input Actuator <UMovementInputActuator>` component, and name it ``ForwardAxis`` 
#. In ``Details`` → ``Actuator Settings``, uncheck :cpp:var:`HasYDimension <UMovementInputActuator::bHasYDimension>` and :cpp:var:`HasZDimension <UMovementInputActuator::bHasZDimension>`.
#. Add an :cpp:class:`Movement Input Actuator <UMovementInputActuator>` component, and name it ``LateralAxis``  
#. In ``Details`` → ``Actuator Settings``, uncheck :cpp:var:`HasXDimension <UMovementInputActuator::bHasXDimension>` and :cpp:var:`HasZDimension <UMovementInputActuator::bHasZDimension>`.  
#. In ``Details`` → ``Actuator Settings``, set :cpp:var:`~UMovementInputActuator::Minspeed` to -1.  

Attaching the Direction and Distance Observer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#. Add three :cpp:class:`Sensor <USensor>` components, and name them ``Teammate Sensor 1``, ``Teammate Sensor 2``, and ``Runner Sensor``.  
#. The ``Target`` variable of each sensor will be set when initializing the environment

Creating the Runner Class
~~~~~~~~~~~~~~~~~~~~~~~~~

The runner is constructed similarly to the tagger but with some minor changes. Please repeat the steps in the :localref:`Creating the Tagger Class` section with the following changes:

#. Add the same :cpp:class:`RayCastSensor <URayCastSensor>` and :cpp:class:`MovementInputActuator <UMovementInputActuator>` to the runner class, but not the ``DirectionDistanceObserver``.
#. Set ``Details`` → ``Character Movement: Walking`` → ``Max Walk Speed`` to 490 cm/s. We will make the runner slower initially to make it easier for the tagger to catch the runner, so the tagger can learn to catch the runner at the beginning of the training. If the runner is as fast or faster than the tagger, the taggers may never catch the runner, preventing the taggers from learning. This can be manually increased during training as the tagger improves and can consistently catch the slower runner.
#. In ``Details`` → ``Tags``, add a new element, and set the value to ``Runner``. This tag is used by the :cpp:class:`RayCastSensor <URayCastSensor>` to detect different objects.

Creating the Environment Definition
-----------------------------------

We will create a ``SetRunnerTagged`` function in the environment which notifies all the trainers when the runner is caught. The :cpp:func:`~IMultiAgentScholaEnvironment::InitializeEnvironment` binds a ``OnActorHit`` Event to each runner, that calls the ``SetRunnerTagged`` function when a runner comes into contact with a tagger. The :cpp:func:`~IMultiAgentScholaEnvironment::Reset` function moves each agent to a random starting location, resets variables, and provides the Initial Agent State at the end of each episode.
The :cpp:func:`~IMultiAgentScholaEnvironment::Step` applies actions, collects observations, computes statuses, rewards, and packages it into the Agent State Structure. Additionally, two optional functions; :cpp:func:`~IMultiAgentScholaEnvironment::SetEnvironmentOptions` and :cpp:func:`~IMultiAgentScholaEnvironment::SeedEnvironment` are provided for logging and reproducability.

#. Create a new Blueprint Class with parent class :cpp:class:`GymConnectorManager <AGymConnectorManager>`, and name it ``TagEnvironment``.  
#. Go to ``Class Settings`` → ``Implemented Interfaces`` and add ``MultiAgentScholaEnvironment``. The five functions should appear in ``Interfaces``.
#. Add a new variable named ``Agents`` of type `Pawn (Object Reference) <https://dev.epicgames.com/documentation/en-us/unreal-engine/pawn-in-unreal-engine>`_ array, and make it publicly editable (by clicking on the eye icon to toggle the visibility). This keeps track of registered agents belonging to this environment definition. 
#. Add an integer variable called ``CurrentStep``, the episode step counter.
#. Add another integer variable named ``MaxSteps``. Make it publicly editable, and set the default value to 2000. This stores the maximum number of steps an episode can run before ending. This may be set to a higher value if the tagger is unable to catch the runner within 2000 steps.
#. Create the ``SetRunnerTagged`` function as shown below.
#. Set the Event Graph as shown below. 

.. blueprint-file:: examples/tag_example/tag_environment/SetRunnerTagged.bp
   :heading: TagEnvironment > SetRunnerTagged
   :imagefallback: /_static/examples/tag_example/tag_environment/SetRunnerTagged.png
   :height: 300
   :zoom: -4

.. blueprint-file:: examples/tag_example/tag_environment/EventGraph-Environment.bp
   :heading: TagEnvironment > Event Graph
   :imagefallback: /_static/examples/tag_example/tag_environment/EventGraph-Environment.png
   :height: 450
   :zoom: -6

Implementing the InitializeEnvironment Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We will now implement the function to define the observation and action spaces for each agent.
To keep the blueprint relatively managable, first create two functions titled ``GetTaggerSpace`` and ``GetRunnerSpace``. Then fill them in as shown below.

.. blueprint-file:: examples/tag_example/tag_environment/GetTaggerSpace.bp
   :heading: TagEnvironment > GetTaggerSpace
   :imagefallback: /_static/examples/tag_example/tag_environment/GetTaggerSpace.png
   :height: 500
   :zoom: -6

.. blueprint-file:: examples/tag_example/tag_environment/GetRunnerSpace.bp
   :heading: TagEnvironment > GetRunnerSpace
   :imagefallback: /_static/examples/tag_example/tag_environment/GetRunnerSpace.png
   :height: 500
   :zoom: -6

Now fill in the :cpp:func:`~IMultiAgentScholaEnvironment::InitializeEnvironment` function:

.. blueprint-file:: examples/tag_example/tag_environment/InitializeEnvironment.bp
   :heading: TagEnvironment > InitializeEnvironment
   :imagefallback: /_static/examples/tag_example/tag_environment/InitializeEnvironment.png
   :height: 450
   :zoom: -6

Implementing the Reset Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We are going to again create some helper functions to keep the reset blueprint itself smaller.
Create three functions titled  ``GetTaggerInitialObs``, ``GetRunnerInitialObs`` and ``CollectInitialObservations``

Fill them in according to the following blueprints.

.. blueprint-file:: examples/tag_example/tag_environment/GetTaggerInitialObs.bp
   :heading: TagEnvironment > GetTaggerInitialObs
   :imagefallback: /_static/examples/tag_example/tag_environment/GetTaggerInitialObs.png
   :height: 500
   :zoom: -6

.. blueprint-file:: examples/tag_example/tag_environment/GetRunnerInitialObs.bp
   :heading: TagEnvironment > GetRunnerInitialObs
   :imagefallback: /_static/examples/tag_example/tag_environment/GetRunnerInitialObs.png
   :height: 500
   :zoom: -6

.. blueprint-file:: examples/tag_example/tag_environment/CollectInitialObservations.bp
   :heading: TagEnvironment > CollectInitialObservations
   :imagefallback: /_static/examples/tag_example/tag_environment/CollectInitialObservations.png
   :height: 500
   :zoom: -6

Now complete the :cpp:func:`~IMultiAgentScholaEnvironment::Reset` function:

.. blueprint-file:: examples/tag_example/tag_environment/Reset.bp
   :heading: TagEnvironment > Reset
   :imagefallback: /_static/examples/tag_example/tag_environment/Reset.png
   :height: 500
   :zoom: -6

Implementing the Step Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For the last of the interface functions, we have the :cpp:func:`~IMultiAgentScholaEnvironment::Step`.
Again, we are going to define several helper functions.

Define the Tagger Reward Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We give a large one-time reward when the tagger agent catches the runner agent, and a small penalty of -0.02 when the tagger agent hits a wall. Additionally, we give a small penalty of -0.005 for each step the tagger agent takes, to encourage the agent to catch the runner agent as quickly as possible. The one-time reward is computed as ``10 - (0.005 * DistanceFromRunner)``, where 10 is the maximum reward for catching the runner, and ``-0.005*DistanceFromRunner`` decreases the reward as the tagger gets further from the runner to ensure taggers near the runner are rewarded more when the runner is caught. The two numbers are chosen based on our experience and can be adjusted as needed. The per-step reward is computed as ``-(0.02*HitWall) - 0.005``.

Create a new function called ``ComputeTaggerReward`` function as shown below.  

.. blueprint-file:: examples/tag_example/tag_environment/ComputeTaggerReward.bp
   :heading: TagEnvironment > ComputeTaggerReward
   :imagefallback: /_static/examples/tag_example/tag_environment/ComputeTaggerReward.png
   :height: 400
   :zoom: -4

Define the Tagger Status Function  
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For taggers, the terminal state is reached when the runner is caught.

Create and set the function ``ComputeTaggerStatus`` as show below.

.. blueprint-file:: examples/tag_example/tag_environment/ComputeTaggerStatus.bp
   :heading: TagEnvironment > ComputeTaggerStatus
   :imagefallback: /_static/examples/tag_example/tag_environment/ComputeTaggerStatus.png
   :height: 400
   :zoom: -4

Tagger Actions and Observations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To apply the incoming actions to the tagger and collect its observations, create and fill in the function ``GetTaggerStepPoints`` as shown below.

.. blueprint-file:: examples/tag_example/tag_environment/GetTaggerStepPoints.bp
   :heading: TagEnvironment > GetTaggerStepPoints
   :imagefallback: /_static/examples/tag_example/tag_environment/GetTaggerStepPoints.png
   :height: 500
   :zoom: -6

Define the Runner Reward Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We give a large one-time penalty of -20 when the runner agent is caught and a small constant per-step reward of 0.01 to encourage the runner to survive as long as possible.

Create the function ``ComputeRunnerReward`` as follows.

.. blueprint-file:: examples/tag_example/tag_environment/ComputeRunnerReward.bp
   :heading: TagEnvironment > ComputeRunnerReward
   :imagefallback: /_static/examples/tag_example/tag_environment/ComputeRunnerReward.png
   :height: 400
   :zoom: -4

Define the Runner Status Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The runner has the same status function as the tagger, except we cast to a Runner object in order to correctly read the ``Caught Target`` variable.

Create ``ComputeRunnerStatus`` as shown below.

.. blueprint-file:: examples/tag_example/tag_environment/ComputeRunnerStatus.bp
   :heading: TagEnvironment > ComputeRunnerStatus
   :imagefallback: /_static/examples/tag_example/tag_environment/ComputeRunnerStatus.png
   :height: 400
   :zoom: -4

Runner Actions and Observations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As we did with the tagger, create and fill in the finction ``GetRunnerStepPoints``.

.. blueprint-file:: examples/tag_example/tag_environment/GetRunnerStepPoints.bp
   :heading: TagEnvironment > GetRunnerStepPoints
   :imagefallback: /_static/examples/tag_example/tag_environment/GetRunnerStepPoints.png
   :height: 500
   :zoom: -6



Now that all of the supplementary functions have been created, fill in the ``Step`` function as follows.

.. blueprint-file:: examples/tag_example/tag_environment/Step.bp
   :heading: TagEnvironment > Step
   :imagefallback: /_static/examples/tag_example/tag_environment/Step.png
   :height: 500
   :zoom: -5

Creating the Map
----------------
  
#. Create a level with a floor and four walls.  
#. Add obstacles and decorations as desired.
#. Place a ``TagEnvironment``  anywhere in the map. The location does not matter.  
#. Place three ``Taggers`` near the centre of the map.
#. Place a ``Runner`` near the taggers.

Registering the Agents
----------------------

1. Select the ``TagEnvironment``  in the map.  
  
   1. Go to ``Details`` panel → ``Default`` → ``Agents``.  
   2. Add 4 new elements, and set the value to the four agents in the map.  
    
2. Select a tagger in the map. 
   
   1. Go to Details Panel.  
   2. Select the ``Teammate Sensor 1`` component, set the ``Target`` to one of the other taggers, and repeat this for ``Teammate Sensor 2``.
   3. Select the ``Runner Sensor`` component, and set the ``Target`` to the runner.
   4. Repeat this for the other two taggers.

Starting Training   
-----------------  
 
We will train the agent using the `Proximal Policy Optimization (PPO) <https://docs.ray.io/en/latest/rllib/rllib-algorithms.html#ppo>`_ algorithm for 2,000,000 steps.
Since `SB3 <https://stable-baselines3.readthedocs.io/>`_ does not support multi-agent training we will use `RLlib <https://docs.ray.io/en/latest/rllib/index.html>`_ for this example. The following two methods run the same training. Running from the terminal may be more convenient for hyperparameter tuning, while running from the Unreal Editor may be more convenient when editing the game.

.. tabs::  
  
   .. group-tab:: Run from terminal  
  
      1. Run the game in Unreal Engine (by clicking the green triangle).  
      2. Open a terminal or command prompt, and run the following Python script:  
  
      .. code-block:: bash  
  
         schola rllib train ppo editor --port 8000 --timesteps 2000000 --use-lstm

      3. Gradually increase the runner's speed in the ``Runner`` Blueprint → ``Character Movement: Walking`` → ``Max Walk Speed`` as the taggers improve and can consistently catch the slower runner.

      .. note::

         The ``--use-lstm`` argument enables an LSTM layer in the RLlib model (see network architecture settings). This gives temporal context to the agent allowing it to track the velocity of other agents, as well as not immediately forget prior observations, which can be crucial in complex environments. Its use is optional. Enabling it improves the agent's ability to navigate around obstacles, but will increase the number of training steps required.

   .. group-tab:: Run from Unreal Editor  

      Schola can also run the training script directly from the Unreal Editor. 
          
      #. Go to the ``Details`` view under the ``TagEnvironment`` actor.
      #. Scroll down to ``Schola``, and head through ``Training`` → ``Connector`` → ``Script Settings``.
      #. Check the :cpp:class:`Run Script on Play <UScholaManagerSubsystemSettings>` box.  
      #. Change :cpp:var:`~UScholaManagerSubsystemSettings::ScriptSettings` → :cpp:var:`~FScriptSettings::RLlibSettings` → :cpp:var:`~FRLlibTrainingSettings::Timesteps` to 2,000,000.
      #. Change :cpp:var:`~UScholaManagerSubsystemSettings::ScriptSettings` → :cpp:var:`~FScriptSettings::RLlibSettings` → :cpp:var:`~FRLlibNetworkArchSettings::bUseAttention` to true.
      #. Run the game in Unreal Engine (by clicking the green triangle).  
      #. Gradually increase the runner's speed in the ``Runner`` Blueprint → ``Character Movement: Walking`` → ``Max Walk Speed`` as the taggers improve and can consistently catch the slower runner is recommended.

      .. note::

         The :cpp:var:`~FRLlibNetworkArchSettings::bUseAttention` setting enables the attention mechanism in the RLlib. This gives temporal context to the agent allowing it to track the velocity of other agents, as well as not immediately forget prior observations, which can be crucial in complex environments. Its use is optional. Enabling it improves the agent's ability to navigate around obstacles, but will increase the number of training steps required.

      .. image:: /_static/examples/tag_example/TagTrainingSettings.png