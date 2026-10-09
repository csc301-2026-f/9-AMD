import schola.generated.Points_pb2 as _Points_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class AgentState(_message.Message):
    __slots__ = ("observations", "reward", "terminated", "truncated", "info")
    class InfoEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    OBSERVATIONS_FIELD_NUMBER: _ClassVar[int]
    REWARD_FIELD_NUMBER: _ClassVar[int]
    TERMINATED_FIELD_NUMBER: _ClassVar[int]
    TRUNCATED_FIELD_NUMBER: _ClassVar[int]
    INFO_FIELD_NUMBER: _ClassVar[int]
    observations: _Points_pb2.Point
    reward: float
    terminated: bool
    truncated: bool
    info: _containers.ScalarMap[str, str]
    def __init__(self, observations: _Optional[_Union[_Points_pb2.Point, _Mapping]] = ..., reward: _Optional[float] = ..., terminated: bool = ..., truncated: bool = ..., info: _Optional[_Mapping[str, str]] = ...) -> None: ...

class EnvironmentState(_message.Message):
    __slots__ = ("agent_states",)
    class AgentStatesEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: AgentState
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[AgentState, _Mapping]] = ...) -> None: ...
    AGENT_STATES_FIELD_NUMBER: _ClassVar[int]
    agent_states: _containers.MessageMap[str, AgentState]
    def __init__(self, agent_states: _Optional[_Mapping[str, AgentState]] = ...) -> None: ...

class TrainingState(_message.Message):
    __slots__ = ("environment_states",)
    ENVIRONMENT_STATES_FIELD_NUMBER: _ClassVar[int]
    environment_states: _containers.RepeatedCompositeFieldContainer[EnvironmentState]
    def __init__(self, environment_states: _Optional[_Iterable[_Union[EnvironmentState, _Mapping]]] = ...) -> None: ...

class InitialAgentState(_message.Message):
    __slots__ = ("observations", "info")
    class InfoEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    OBSERVATIONS_FIELD_NUMBER: _ClassVar[int]
    INFO_FIELD_NUMBER: _ClassVar[int]
    observations: _Points_pb2.Point
    info: _containers.ScalarMap[str, str]
    def __init__(self, observations: _Optional[_Union[_Points_pb2.Point, _Mapping]] = ..., info: _Optional[_Mapping[str, str]] = ...) -> None: ...

class InitialEnvironmentState(_message.Message):
    __slots__ = ("agent_states",)
    class AgentStatesEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: InitialAgentState
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[InitialAgentState, _Mapping]] = ...) -> None: ...
    AGENT_STATES_FIELD_NUMBER: _ClassVar[int]
    agent_states: _containers.MessageMap[str, InitialAgentState]
    def __init__(self, agent_states: _Optional[_Mapping[str, InitialAgentState]] = ...) -> None: ...

class InitialState(_message.Message):
    __slots__ = ("environment_states",)
    class EnvironmentStatesEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: int
        value: InitialEnvironmentState
        def __init__(self, key: _Optional[int] = ..., value: _Optional[_Union[InitialEnvironmentState, _Mapping]] = ...) -> None: ...
    ENVIRONMENT_STATES_FIELD_NUMBER: _ClassVar[int]
    environment_states: _containers.MessageMap[int, InitialEnvironmentState]
    def __init__(self, environment_states: _Optional[_Mapping[int, InitialEnvironmentState]] = ...) -> None: ...

class State(_message.Message):
    __slots__ = ("training_state", "initial_state")
    TRAINING_STATE_FIELD_NUMBER: _ClassVar[int]
    INITIAL_STATE_FIELD_NUMBER: _ClassVar[int]
    training_state: TrainingState
    initial_state: InitialState
    def __init__(self, training_state: _Optional[_Union[TrainingState, _Mapping]] = ..., initial_state: _Optional[_Union[InitialState, _Mapping]] = ...) -> None: ...
