import schola.generated.Spaces_pb2 as _Spaces_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class AgentDefinition(_message.Message):
    __slots__ = ("obs_space", "action_space", "agent_type")
    OBS_SPACE_FIELD_NUMBER: _ClassVar[int]
    ACTION_SPACE_FIELD_NUMBER: _ClassVar[int]
    AGENT_TYPE_FIELD_NUMBER: _ClassVar[int]
    obs_space: _Spaces_pb2.Space
    action_space: _Spaces_pb2.Space
    agent_type: str
    def __init__(self, obs_space: _Optional[_Union[_Spaces_pb2.Space, _Mapping]] = ..., action_space: _Optional[_Union[_Spaces_pb2.Space, _Mapping]] = ..., agent_type: _Optional[str] = ...) -> None: ...

class EnvironmentDefinition(_message.Message):
    __slots__ = ("agent_definitions",)
    class AgentDefinitionsEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: AgentDefinition
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[AgentDefinition, _Mapping]] = ...) -> None: ...
    AGENT_DEFINITIONS_FIELD_NUMBER: _ClassVar[int]
    agent_definitions: _containers.MessageMap[str, AgentDefinition]
    def __init__(self, agent_definitions: _Optional[_Mapping[str, AgentDefinition]] = ...) -> None: ...

class TrainingDefinition(_message.Message):
    __slots__ = ("environment_definitions",)
    ENVIRONMENT_DEFINITIONS_FIELD_NUMBER: _ClassVar[int]
    environment_definitions: _containers.RepeatedCompositeFieldContainer[EnvironmentDefinition]
    def __init__(self, environment_definitions: _Optional[_Iterable[_Union[EnvironmentDefinition, _Mapping]]] = ...) -> None: ...
