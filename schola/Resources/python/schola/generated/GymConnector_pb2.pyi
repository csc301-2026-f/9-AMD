import schola.generated.Definitions_pb2 as _Definitions_pb2
import schola.generated.State_pb2 as _State_pb2
import schola.generated.StateUpdates_pb2 as _StateUpdates_pb2
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class AutoResetType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = ()
    SAME_STEP: _ClassVar[AutoResetType]
    NEXT_STEP: _ClassVar[AutoResetType]
    DISABLED: _ClassVar[AutoResetType]
SAME_STEP: AutoResetType
NEXT_STEP: AutoResetType
DISABLED: AutoResetType

class TrainingDefinitionRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class GymConnectorStartRequest(_message.Message):
    __slots__ = ("autoreset_type",)
    AUTORESET_TYPE_FIELD_NUMBER: _ClassVar[int]
    autoreset_type: AutoResetType
    def __init__(self, autoreset_type: _Optional[_Union[AutoResetType, str]] = ...) -> None: ...

class GymConnectorStartResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class InitialStateRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...
