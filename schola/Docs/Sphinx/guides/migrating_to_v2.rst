Migrating from Schola V1.3 to V2.1
==================================

Schola V2 introduces significant improvements to the API, including better separation of concerns, more flexible environment interfaces, and improved Python integration. This guide will help you migrate your existing Schola V1.3 projects to V2.


Overview of Changes
-------------------

The major changes in Schola V2 include:

* **Unified Environment Interfaces**: Single and multi-agent environments now use a consistent interface with better type safety
* **Protocol-Simulator Architecture**: Python API now separates communication protocols from simulation management
* **New Actuators & Sensors**: Refactored component-based actuators and sensors with cleaner interfaces
* **Improved Training Settings**: More structured and flexible training configuration
* **Better Imitation Learning Support**: Dedicated interfaces for imitation learning workflows

Python API Changes
------------------

Environment Connection
~~~~~~~~~~~~~~~~~~~~~~

**V1.3 Approach:**

.. code-block:: python

   from schola import UnrealConnection
   
   connection = UnrealConnection(port=50051)
   env = GymEnv(connection)

**V2 Approach:**

.. code-block:: python

   from schola.core.simulators.unreal import UnrealEditor
   from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
   from schola.gym.env import GymEnv
   
   # Define protocol (communication layer)
   protocol = GrpcProtocol(url="localhost", port=50051)
   
   # Define simulator (Unreal Engine management)
   simulator = UnrealEditor()
   
   # Create environment
   env = GymEnv(simulator=simulator, protocol=protocol)

The new architecture separates:

* **Protocol**: How to communicate with Unreal (gRPC, shared memory, etc.)
* **Simulator**: How to manage the Unreal Engine process (editor, executable, etc.)

This allows mixing and matching different protocols and simulators as needed.

CLI Changes
~~~~~~~~~~~

**V1.3 Commands:**

.. code-block:: bash

   # Stable Baselines 3
   schola-sb3 PPO --learning-rate 0.0003 --n-steps 2048
   schola-sb3 SAC --buffer-size 1000000
   
   # RLlib
   schola-rllib PPO --learning-rate 0.0003

**V2 Commands:**

.. code-block:: bash

   # Stable Baselines 3 (``editor`` = attach to a running Unreal Editor session)
   schola sb3 train ppo editor --learning-rate 0.0003 --n-steps 2048
   schola sb3 train sac editor --buffer-size 1000000
   
   # RLlib
   schola rllib train ppo editor --learning-rate 0.0003
   
   # Or using module invocation (same nested tokens as the ``schola`` entry point)
   python -m schola.scripts.launch sb3 train ppo editor --learning-rate 0.0003 --n-steps 2048
   python -m schola.scripts.launch rllib train ppo editor --learning-rate 0.0003

Key changes:

* V1.3 used separate entry points (``schola-sb3``, ``schola-rllib``)
* V2 uses a unified ``schola`` command with subcommands
* V2 adds explicit ``train`` subcommand for better organization
* Algorithm is now a subcommand for sb3 (``ppo``, ``sac``) and RLlib (``ppo``, ``sac``, ``impala``, ``appo``), followed by a simulator subcommand (``editor``, ``executable``, or ``project``)

Vectorized Environments
~~~~~~~~~~~~~~~~~~~~~~~

**V1.3:**

.. code-block:: python

   from schola import GymVectorEnv
   connection = ...
   env = GymVectorEnv(connection, num_envs=4)

**V2:**

.. code-block:: python

   from schola.gym.env import GymVectorEnv
   simulator = ...
   protocol = ...
   # Environment vectorization is now handled by Unreal internally
   # Just connect to an environment with multiple instances
   env = GymVectorEnv(simulator=simulator, protocol=protocol)

Auto-Reset Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

**V1.3:**

In V1.3, auto-reset behavior was implicit and always enabled for vectorized environments. There was no explicit configuration needed:

.. code-block:: python

   from schola.gym import VecEnv
   from schola.core.unreal_connections import UnrealEditorConnection
   
   # Auto-reset was automatically enabled for VecEnv
   connection = UnrealEditorConnection("localhost", port=8002)
   env = VecEnv(connection)  # Auto-reset implicitly enabled

**V2:**

In V2, auto-reset behavior is **explicitly configured** when creating the environment:

.. code-block:: python

   from schola.gym.env import GymVectorEnv
   from schola.core.simulators.unreal import UnrealEditor
   from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
   from gymnasium.vector.vector_env import AutoresetMode
   
   protocol = GrpcProtocol(url="localhost", port=50051)
   simulator = UnrealEditor()
   
   # For vectorized environments - explicitly specify autoreset_mode
   env = GymVectorEnv(
       simulator=simulator, 
       protocol=protocol,
       autoreset_mode=AutoresetMode.SAME_STEP  # Default, but now explicit
   )
   
   # For single-agent GymEnv, autoreset is always DISABLED
   env_single = GymEnv(simulator=simulator, protocol=protocol)

**Key Differences:**

* **V1.3**: Auto-reset was implicit and always-on for vectorized environments
* **V2**: Auto-reset is an explicit parameter with three modes:

  * ``DISABLED``: No automatic reset. You must call ``env.reset()`` manually when episodes end
  * ``SAME_STEP``: Automatically resets and returns first observation of new episode in the same step (default)
  * ``NEXT_STEP``: Resets with the next step (not commonly used)

**Why This Matters:**

V2's explicit configuration gives you more control over environment behavior, especially when debugging or wanting manual control over episode boundaries.

Unreal Engine API Changes
--------------------------

Environment Interface
~~~~~~~~~~~~~~~~~~~~~

The environment interface has been unified and improved for both single and multi-agent scenarios.

**V1.3 Multi-Agent Interface:**

.. code-block:: cpp

   UFUNCTION(BlueprintNativeEvent)
   void RegisterAgents(TMap<FString, FAgentDefinition>& OutDefinitions);
   
   UFUNCTION(BlueprintNativeEvent)
   void Reset(TMap<FString, FObservation>& OutObservations);
   
   UFUNCTION(BlueprintNativeEvent)
   void Step(const TMap<FString, FAction>& Actions,
             TMap<FString, FObservation>& OutObservations,
             TMap<FString, float>& OutRewards,
             TMap<FString, bool>& OutDones);

**V2 Multi-Agent Interface:**

.. code-block:: cpp

   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
   void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions);
   
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
   void Reset(TMap<FString, FInitialAgentState>& OutAgentState);
   
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
   void Step(const TMap<FString, FInstancedStruct>& InActions,
             TMap<FString, FAgentState>& OutAgentStates);

Key changes:

1. **InitializeEnvironment**: Now separate from first reset, includes both observation and action space definitions via ``FInteractionDefinition``
2. **FAgentState**: Consolidated structure containing observations, rewards, terminated, truncated, and info
3. **FInitialAgentState**: Separate structure for reset that includes observations and info only
4. **Terminated vs Truncated**: Now properly separated following Gymnasium conventions

Base Interface - Inheritance vs Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

V1.3 used an **inheritance-based** approach with `AAbstractScholaEnvironment` (an Actor), while V2 uses an **interface-based** approach with `IBaseScholaEnvironment`. This is a fundamental architectural change that also eliminates the separate Trainer system.

**V1.3 Approach (Inheritance from Actor with Trainers):**

In V1.3, environments managed agents through an `AAbstractTrainer` system:

.. code-block:: cpp

   // V1.3 - Inherit from AAbstractScholaEnvironment Actor
   UCLASS()
   class MYGAME_API AMyEnvironment : public AAbstractScholaEnvironment
   {
       GENERATED_BODY()
       
   protected:
       // V1.3 stored trainers internally
       TMap<int, AAbstractTrainer*> Trainers;
       
   public:
       // Override virtual functions from base class
       virtual void RegisterAgents() override;  // Registered trainers with environment
       virtual void ResetEnvironment() override;
       virtual void InitializeEnvironment() override;
   };
   
   // Separate AAbstractTrainer classes handled agent logic
   UCLASS()
   class MYGAME_API AMyTrainer : public AAbstractTrainer
   {
       GENERATED_BODY()
       
   public:
       virtual void ComputeReward() override;
       virtual void ComputeStatus() override;
   };

The V1.3 architecture separated concerns:

* **Environment** (`AAbstractScholaEnvironment`): Managed the world state
* **Trainer** (`AAbstractTrainer`): Handled per-agent reward/status logic
* **Static vs Dynamic**: Different environment types for fixed vs. runtime agent spawning

**V2 Approach (Unified Interface - No Trainers):**

In V2, the Trainer system is **removed**. The environment directly handles all logic including rewards and episode status:

.. code-block:: cpp

   // V2 - Implement IMultiAgentScholaEnvironment interface on ANY Actor or Object
   UCLASS()
   class MYGAME_API AMyEnvironment : public AActor, public IMultiAgentScholaEnvironment
   {
       GENERATED_BODY()
       
   public:
       // Implement BlueprintNativeEvent functions from interface
       virtual void InitializeEnvironment_Implementation(
           TMap<FString, FInteractionDefinition>& OutAgentDefinitions) override;
       
       virtual void Reset_Implementation(
           TMap<FString, FInitialAgentState>& OutAgentState) override;
       
       // Step now handles EVERYTHING: observations, rewards, terminated, truncated
       virtual void Step_Implementation(
           const TMap<FString, FInstancedStruct>& InActions,
           TMap<FString, FAgentState>& OutAgentStates) override
       {
           // 1. Apply actions to agents
           // 2. Update world state
           // 3. Collect observations
           // 4. Compute rewards
           // 5. Check termination/truncation
           // 6. Populate OutAgentStates with all of the above
       }
       
       virtual void SeedEnvironment_Implementation(int Seed) override;
       
       virtual void SetEnvironmentOptions_Implementation(
           const TMap<FString, FString>& Options) override;
   };

The V2 architecture simplifies to:

* **Environment Interface Only**: All logic (observation, action, reward, termination) in one place
* **No Trainer Layer**: Reward and status computation happens directly in `Step()`
* **Unified Agent Handling**: No distinction between static/dynamic - all use the same interface

**Blueprint Implementation:**

V1.3 Blueprint:

* Inherited from ``AbstractScholaEnvironment`` Blueprint class
* Created separate ``AbstractTrainer`` Blueprint classes for each agent type
* Environment: Overrode ``Register Agents`` event to register trainers
* Trainer: Implemented ``Compute Reward`` and ``Compute Status`` events

V2 Blueprint:

* Can use **any** Actor Blueprint as the base (no fixed inheritance)
* Add ``MultiAgentScholaEnvironment`` interface in Class Settings
* **No separate Trainer classes needed**
* Implement all logic in environment interface events:
  
  * ``Initialize Environment`` - Define observation and action spaces
  * ``Reset`` - Reset environment and return initial observations
  * ``Step`` - Apply actions, compute observations, rewards, and termination **all in one place**
  * ``Seed Environment`` - Handle seeding
  * ``Set Environment Options`` - Handle configuration options

**Key Advantages of V2's Interface Approach:**

1. **Flexibility**: Your environment can inherit from any Actor class (APawn, ACharacter, custom base class)
2. **Multiple Interfaces**: Can implement both training and imitation interfaces
3. **Simpler Architecture**: No separate Trainer layer - all logic in one place
4. **Cleaner Separation**: Environment logic is not tied to a specific base class
5. **Better for Blueprints**: Easier to add Schola to existing Blueprint hierarchies
6. **Unified Agent Handling**: No more Static vs Dynamic environment distinction

**Interface Discovery:**

.. code-block:: cpp

   // V2 - The GymConnector finds environments by the base interface
   UINTERFACE(BlueprintType, Blueprintable)
   class UBaseScholaEnvironment : public UInterface
   {
       GENERATED_BODY()
   };
   
   // Implement specific interface for your use case:
   
   // Single agent environments
   UINTERFACE(BlueprintType, Blueprintable)
   class USingleAgentScholaEnvironment : public UBaseScholaEnvironment { /*...*/ };
   
   // Multi-agent environments
   UINTERFACE(BlueprintType, Blueprintable)
   class UMultiAgentScholaEnvironment : public UBaseScholaEnvironment { /*...*/ };

New Functions
~~~~~~~~~~~~~

V2 adds several new environment functions:

.. code-block:: cpp

   // Seed the environment for reproducibility
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
   void SeedEnvironment(int Seed);
   
   // Set environment options from Python
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
   void SetEnvironmentOptions(const TMap<FString, FString>& Options);

Actuators and Sensors
~~~~~~~~~~~~~~~~~~~~~

Actuators and sensors are now interfaces that can be implemented by any class, not just as actor components.

**V1.3 Actuator Pattern:**

.. code-block:: cpp

   UCLASS()
   class UMyActuator : public UActorComponent
   {
       // Custom implementation
   };

**V2 Actuator Pattern:**

.. code-block:: cpp

   UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
   class UMyActuator : public UActorComponent, public IScholaActuator
   {
       GENERATED_BODY()
       
   public:
       // Interface implementation
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Actuator")
       void GetActionSpace(FInstancedStruct& OutActionSpace) const;
       
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Actuator")
       void TakeAction(const FInstancedStruct& Action);
       
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Actuator")
       void InitActuator();
   };

**V2 Sensor Pattern:**

.. code-block:: cpp

   UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
   class UMySensor : public USceneComponent, public IScholaSensor
   {
       GENERATED_BODY()
       
   public:
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Sensor")
       void CollectObservations(FInstancedStruct& OutObservations);
       
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Sensor")
       void GetObservationSpace(FInstancedStruct& OutObservationSpace) const;
       
       UFUNCTION(BlueprintNativeEvent, Category = "Schola|Sensor")
       void InitSensor();
   };


Gym Connector Changes
~~~~~~~~~~~~~~~~~~~~~

**V1.3 Setup:**

In v1.3 and earlier, the gym connector was managed by a hidden Subsystem. In v2, it is now an interactable object, giving you direct control over the lifecycle and timing of the connector. 

.. Note::
   If you just want to drop a gym connector into the scene, you can use the :cpp:class:`UGymConnectorManager`, which will step the connector every tick and start it when the level starts.


**V2 Setup:**

.. code-block:: cpp

   // More specific connector type
   UPROPERTY(EditAnywhere)
   URPCGymConnector* GymConnector;  // Attach to some object in the scene 

**V2 Connector Settings:**
Previously, the connector settings were available through the plugin settings menu, now they are available on the GymConnector itself.

.. code-block:: cpp

   UPROPERTY(EditAnywhere, Category = "Schola|gRPC")
   FRPCServerSettings ServerSettings;  // Port, address configuration
   
   UPROPERTY(EditAnywhere, Category = "Script Settings")
   FScriptSettings ScriptSettings;  // Python script launch configuration
   
   UPROPERTY(EditAnywhere, Category = "External Gym Connector Settings")
   FExternalGymConnectorSettings ExternalSettings;  // Timeout settings

Initialization
~~~~~~~~~~~~~~

**V1.3:**

.. code-block:: cpp

   GymConnector->Initialize(Environments);

**V2:**

.. code-block:: cpp

   // Initialize with array of environment interfaces
   TArray<TScriptInterface<IBaseScholaEnvironment>> Environments;
   GymConnector->Init(Environments);

Training Settings
~~~~~~~~~~~~~~~~~

Training settings are now more structured with separate configuration objects.

**V2 RLlib Settings Structure:**

.. code-block:: cpp

   UPROPERTY(EditAnywhere)
   FRLlibTrainingSettings TrainingSettings;
   {
       FRLlibPPOSettings AlgorithmSettings;
       FRLlibNetworkArchitectureSettings NetworkArchitectureSettings;
       FRLlibCheckpointSettings CheckpointSettings;
       FRLlibLoggingSettings LoggingSettings;
       FRLlibResourceSettings ResourceSettings;
       FRLlibResumeSettings ResumeSettings;
   };

**V2 Stable Baselines 3 Settings Structure:**

.. code-block:: cpp

   UPROPERTY(EditAnywhere)
   FSB3TrainingSettings TrainingSettings;
   {
       FSB3PPOSettings AlgorithmSettings;
       FSB3NetworkArchitectureSettings NetworkArchitectureSettings;
       FSB3CheckpointSettings CheckpointSettings;
       FSB3LoggingSettings LoggingSettings;
       FSB3ResumeSettings ResumeSettings;
   };

Points and Spaces
~~~~~~~~~~~~~~~~~

The Points and Spaces API remains largely compatible, but now uses ``FInstancedStruct`` and ``TInstancedStruct`` as oposed to ``TVariant``.

**V2:**

.. code-block:: cpp

   // Using instanced structs for type erasure
   FInstancedStruct ActionSpace;  // Contains FBoxSpace
   FInstancedStruct Action;       // Contains FBoxPoint
   
   // Access typed values
   const FBoxSpace& BoxSpace = ActionSpace.Get<FBoxSpace>();
   const FBoxPoint& BoxPoint = Action.Get<FBoxPoint>

Imitation Learning
------------------

V2 introduces dedicated interfaces for imitation learning (behavior cloning).

**Imitation Environment Interface:**

.. note::
   :cpp:class:`UAbstractImitationConnector` is not currently compatible with :cpp:class:`UGymConnectorManager`, you must initialize and step the connector manually.

**Python Imitation API:**

.. code-block:: python

   from schola.minari.datacollector import ScholaDataCollector
   from schola.core.protocols.protobuf.offline_grpc_protocol import GrpcImitationProtocol
   from schola.core.simulators.unreal import UnrealEditor
   
   protocol = GrpcImitationProtocol(url="localhost", port=50051)
   simulator = UnrealEditor()
   
   collector = ScholaDataCollector(protocol, simulator, seed = 123)

   for i in range(10):
      collector.step()
   
   schola_dataset = collector.create_dataset("dataset_name")

Migration Checklist
-------------------

Use this checklist to ensure you've covered all necessary changes:

Python Code
~~~~~~~~~~~

☐ Replace ``UnrealConnection`` with ``protocol`` + ``simulator`` pattern

☐ Update import statements to use new module structure

☐ Update CLI commands to use ``schola`` entry point

☐ Replace ``auto_reset`` settings with ``AutoresetMode`` enum

☐ Update vectorized environment usage

☐ Update any custom protocol/connection code

Unreal Engine Code (C++)
~~~~~~~~~~~~~~~~~~~~~~~~~

☐ Update environment base class from ``AAbstractScholaEnvironment`` to ``IMultiAgentScholaEnvironment`` or ``ISingleAgentScholaEnvironment`` interface

☐ **Remove all Trainer classes** (``AAbstractTrainer`` and subclasses are no longer used)

☐ **Migrate Trainer logic** into environment's ``Step()`` function:

   * Move ``ComputeReward()`` logic into ``Step()``
   * Move ``ComputeStatus()`` logic into ``Step()``
   * Consolidate all per-agent logic in one place

☐ Replace ``RegisterAgents`` with ``InitializeEnvironment``

☐ Update ``Reset`` signature to return ``FInitialAgentState``

☐ Update ``Step`` signature to use ``FAgentState`` (includes observations, rewards, terminated, truncated)

☐ Add ``SeedEnvironment`` implementation

☐ Add ``SetEnvironmentOptions`` implementation

☐ Split ``Done`` flag into ``Terminated`` and ``Truncated``

☐ Update connector type from ``UGymConnector`` to specific type (e.g., ``URPCGymConnector``)

☐ Update connector initialization to use ``Init()``

☐ Update training settings to use structured settings objects

Unreal Engine Blueprints
~~~~~~~~~~~~~~~~~~~~~~~~~

☐ Update environment Blueprint to implement ``MultiAgentScholaEnvironment`` or ``SingleAgentScholaEnvironment`` interface

☐ **Delete all Trainer Blueprints** (Trainer classes are no longer needed)

☐ **Migrate Trainer logic** into environment's ``Step`` event:

   * Move reward calculation from Trainer into environment ``Step``
   * Move episode status logic from Trainer into environment ``Step``
   * Remove ``Register Agents`` event that registered trainers

☐ Update actuator components to implement ``IScholaActuator``

☐ Update sensor components to implement ``IScholaSensor``

☐ Replace custom actuators with built-in ones where possible

☐ Update agent state structures in Blueprint nodes

☐ Update ``Done`` logic to separate ``Terminated`` and ``Truncated``

Common Migration Issues
-----------------------

Issue: "Interface not found"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Problem**: Environment doesn't appear in Schola's environment list

**Solution**: Ensure your environment implements ``IBaseScholaEnvironment`` (either through ``ISingleAgentScholaEnvironment`` or ``IMultiAgentScholaEnvironment``)

Issue: "Type mismatch with FInstancedStruct"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Problem**: Compiler errors when working with Points and Spaces

**Solution**: Use ``FInstancedStruct`` wrapper and access typed values with ``.Get<T>()``

.. code-block:: cpp

   // Correct V2 approach
   FInstancedStruct ActionStruct;
   ActionStruct.InitializeAs<FBoxPoint>();
   FBoxPoint& Action = ActionStruct.GetMutable<FBoxPoint>();

Issue: "Environment step not called"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Problem**: Step function isn't being invoked

**Solution**: 

1. Verify connector is properly initialized with ``Init()``
2. Check that Python script sent startup message
3. Ensure environment is in the connector's environment list

Issue: "Python connection timeout"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Problem**: Python script times out connecting to Unreal

**Solution**:

1. Verify port numbers match between Unreal and Python
2. Increase ``environment_start_timeout`` in ``GrpcProtocolConfig`` (or deprecated ``gRPCProtocolArgs``)
3. Check firewall settings

Additional Resources
--------------------

* :doc:`setup_schola` - Setting up Schola V2 from scratch
* :doc:`running_schola` - Running training with V2

For more detailed API documentation, see:

* :doc:`../API/index` - 

.. note::
   If you encounter issues not covered in this guide, please check the `Schola GitHub repository <https://github.com/GPUOpen-LibrariesAndSDKs/Schola>`_ for updates and open an issue if needed.

