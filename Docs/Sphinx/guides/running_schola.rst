Running Schola
==============

Schola V2 uses a protocol-simulator architecture to interact with Unreal Engine. It supports both creating a standalone instance of Unreal Engine 
running as a child process of your Python script, or connecting to a running Unreal Engine process. The architecture separates:

* **Protocols** (:py:class:`~schola.core.protocols.base_protocol.BaseRLProtocol`) - Handle communication with Unreal Engine
* **Simulators** (:py:class:`~schola.core.simulators.base_simulator.BaseSimulator`) - Manage the Unreal Engine process lifecycle


Launch An Unreal Environment From Python
----------------------------------------

Schola supports running environments entirely from Python using the :py:class:`~schola.core.simulators.unreal.UnrealExecutable` simulator 
combined with a communication protocol like :py:class:`~schola.core.protocols.protobuf.grpc_protocol.GrpcProtocol`.

.. code-block:: python

    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal import UnrealExecutable

    # Define the communication protocol
    protocol = GrpcProtocol(
        url="localhost",     # Connect to the engine over localhost
        port=50051           # Port for gRPC communication (default: 50051)
    )

    # Define the simulator to manage the Unreal Engine executable
    simulator = UnrealExecutable(
        executable_path="Path/To/Your/Game.exe",  # Path to your packaged game binary
        headless_mode=True,          # Skip rendering (faster training)
        display_logs=True,           # Show Unreal Engine logs in terminal
        map=None,                    # Use default map, or specify "/Game/<MapName>"
        set_fps=60,                  # Fixed framerate for deterministic training
        disable_script=True          # Ignore RunScriptOnLaunch setting
    )


Initialize the Standalone Environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::

    .. group-tab:: Gymnasium
        
        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealExecutable
            from schola.gym.env import GymVectorEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealExecutable(
                executable_path="Path/To/Your/Game.exe",
                headless_mode=True
            )

            # Create vectorized Gymnasium environment
            env = GymVectorEnv(simulator=simulator, protocol=protocol)
            
            # Use the environment
            obs, info = env.reset()
            actions = env.action_space.sample()
            obs, rewards, terminated, truncated, info = env.step(actions)

    .. group-tab:: Ray RLlib
        
        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealExecutable
            from schola.rllib import RayEnv, RayVecEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealExecutable(
                executable_path="Path/To/Your/Game.exe",
                headless_mode=True
            )

            # For single environment (local runner)
            env = RayEnv(protocol=protocol, simulator=simulator)
            
            # For multiple parallel environments (distributed training)
            # env = RayVecEnv(protocol=protocol, simulator=simulator)
            
            # Use the environment
            obs, info = env.reset()
            actions = {agent_id: env.action_space.sample() for agent_id in env.get_agent_ids()}
            obs, rewards, terminated, truncated, info = env.step(actions)

    .. group-tab:: Stable Baselines 3

        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealExecutable
            from schola.sb3 import VecEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealExecutable(
                executable_path="Path/To/Your/Game.exe",
                headless_mode=True
            )

            # Create vectorized environment for SB3
            env = VecEnv(simulator=simulator, protocol=protocol)
            
            # Use with SB3 algorithms
            from stable_baselines3 import PPO
            model = PPO("MultiInputPolicy", env, verbose=1)
            model.learn(total_timesteps=10000)


Connect To a Running Unreal Environment
---------------------------------------

Schola supports connecting to an already running Editor or Game, for debugging and Unreal Engine driven workflows using the :py:class:`~schola.core.simulators.unreal.UnrealEditor` simulator.

.. code-block:: python

    from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
    from schola.core.simulators.unreal import UnrealEditor

    # Define the communication protocol
    protocol = GrpcProtocol(
        url="localhost",  # Connect to the engine over localhost
        port=50051        # Must match the port in your Unreal Engine Schola Plugin Settings
    )
    
    # Define the simulator (no process management needed for editor)
    simulator = UnrealEditor()

Initialize the Editor Environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::

    .. group-tab:: Gymnasium
        
        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealEditor
            from schola.gym.env import GymVectorEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealEditor()

            # Create vectorized Gymnasium environment
            env = GymVectorEnv(simulator=simulator, protocol=protocol)
            
            # Use the environment
            obs, info = env.reset()
            actions = env.action_space.sample()
            obs, rewards, terminated, truncated, info = env.step(actions)

    .. group-tab:: Ray RLlib
        
        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealEditor
            from schola.rllib import RayEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealEditor()

            # Create Ray environment
            env = RayEnv(protocol=protocol, simulator=simulator)
            
            # Use the environment
            obs, info = env.reset()
            actions = {agent_id: env.action_space.sample() for agent_id in env.get_agent_ids()}
            obs, rewards, terminated, truncated, info = env.step(actions)

    .. group-tab:: Stable Baselines 3

        .. code-block:: python

            from schola.core.protocols.protobuf.grpc_protocol import GrpcProtocol
            from schola.core.simulators.unreal import UnrealEditor
            from schola.sb3 import VecEnv

            # Setup protocol and simulator
            protocol = GrpcProtocol(url="localhost", port=50051)
            simulator = UnrealEditor()

            # Create vectorized environment for SB3
            env = VecEnv(simulator=simulator, protocol=protocol)
            
            # Use with SB3 algorithms
            from stable_baselines3 import PPO
            model = PPO("MultiInputPolicy", env, verbose=1)
            model.learn(total_timesteps=10000)