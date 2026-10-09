import schola.generated.Points_pb2 as _Points_pb2
import schola.generated.State_pb2 as _State_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ImitationAgentState(_message.Message):
    __slots__ = ("observations", "reward", "terminated", "truncated", "info", "actions")
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
    ACTIONS_FIELD_NUMBER: _ClassVar[int]
    observations: _Points_pb2.Point
    reward: float
    terminated: bool
    truncated: bool
    info: _containers.ScalarMap[str, str]
    actions: _Points_pb2.Point
    def __init__(self, observations: _Optional[_Union[_Points_pb2.Point, _Mapping]] = ..., reward: _Optional[float] = ..., terminated: bool = ..., truncated: bool = ..., info: _Optional[_Mapping[str, str]] = ..., actions: _Optional[_Union[_Points_pb2.Point, _Mapping]] = ...) -> None: ...

class ImitationEnvironmentState(_message.Message):
    __slots__ = ("agent_states",)
    class AgentStatesEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: ImitationAgentState
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[ImitationAgentState, _Mapping]] = ...) -> None: ...
    AGENT_STATES_FIELD_NUMBER: _ClassVar[int]
    agent_states: _containers.MessageMap[str, ImitationAgentState]
    def __init__(self, agent_states: _Optional[_Mapping[str, ImitationAgentState]] = ...) -> None: ...

class ImitationTrainingState(_message.Message):
    __slots__ = ("environment_states",)
    ENVIRONMENT_STATES_FIELD_NUMBER: _ClassVar[int]
    environment_states: _containers.RepeatedCompositeFieldContainer[ImitationEnvironmentState]
    def __init__(self, environment_states: _Optional[_Iterable[_Union[ImitationEnvironmentState, _Mapping]]] = ...) -> None: ...

class ImitationState(_message.Message):
    __slots__ = ("training_state", "initial_state")
    TRAINING_STATE_FIELD_NUMBER: _ClassVar[int]
    INITIAL_STATE_FIELD_NUMBER: _ClassVar[int]
    training_state: ImitationTrainingState
    initial_state: _State_pb2.InitialState
    def __init__(self, training_state: _Optional[_Union[ImitationTrainingState, _Mapping]] = ..., initial_state: _Optional[_Union[_State_pb2.InitialState, _Mapping]] = ...) -> None: ...
