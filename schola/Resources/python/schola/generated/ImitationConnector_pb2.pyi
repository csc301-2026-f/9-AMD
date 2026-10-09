import schola.generated.ImitationState_pb2 as _ImitationState_pb2
import schola.generated.StateUpdates_pb2 as _StateUpdates_pb2
import schola.generated.Definitions_pb2 as _Definitions_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ImitationConnectorStartRequest(_message.Message):
    __slots__ = ("environments",)
    class EnvironmentsEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: int
        value: _StateUpdates_pb2.EnvironmentSettings
        def __init__(self, key: _Optional[int] = ..., value: _Optional[_Union[_StateUpdates_pb2.EnvironmentSettings, _Mapping]] = ...) -> None: ...
    ENVIRONMENTS_FIELD_NUMBER: _ClassVar[int]
    environments: _containers.MessageMap[int, _StateUpdates_pb2.EnvironmentSettings]
    def __init__(self, environments: _Optional[_Mapping[int, _StateUpdates_pb2.EnvironmentSettings]] = ...) -> None: ...

class ImitationConnectorStartResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class ImitationStateRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class ImitationDefinitionRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...
