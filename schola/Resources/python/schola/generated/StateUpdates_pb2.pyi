import schola.generated.Points_pb2 as _Points_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable, Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class CommunicatorStatus(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = ()
    GOOD: _ClassVar[CommunicatorStatus]
    ERROR: _ClassVar[CommunicatorStatus]
    CLOSED: _ClassVar[CommunicatorStatus]
GOOD: CommunicatorStatus
ERROR: CommunicatorStatus
CLOSED: CommunicatorStatus

class EnvironmentStep(_message.Message):
    __slots__ = ("updates",)
    class UpdatesEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: _Points_pb2.Point
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[_Points_pb2.Point, _Mapping]] = ...) -> None: ...
    UPDATES_FIELD_NUMBER: _ClassVar[int]
    updates: _containers.MessageMap[str, _Points_pb2.Point]
    def __init__(self, updates: _Optional[_Mapping[str, _Points_pb2.Point]] = ...) -> None: ...

class EnvironmentSettings(_message.Message):
    __slots__ = ("seed", "options")
    class OptionsEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: str
        def __init__(self, key: _Optional[str] = ..., value: _Optional[str] = ...) -> None: ...
    SEED_FIELD_NUMBER: _ClassVar[int]
    OPTIONS_FIELD_NUMBER: _ClassVar[int]
    seed: int
    options: _containers.ScalarMap[str, str]
    def __init__(self, seed: _Optional[int] = ..., options: _Optional[_Mapping[str, str]] = ...) -> None: ...

class Reset(_message.Message):
    __slots__ = ("environments",)
    class EnvironmentsEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: int
        value: EnvironmentSettings
        def __init__(self, key: _Optional[int] = ..., value: _Optional[_Union[EnvironmentSettings, _Mapping]] = ...) -> None: ...
    ENVIRONMENTS_FIELD_NUMBER: _ClassVar[int]
    environments: _containers.MessageMap[int, EnvironmentSettings]
    def __init__(self, environments: _Optional[_Mapping[int, EnvironmentSettings]] = ...) -> None: ...

class Step(_message.Message):
    __slots__ = ("environments",)
    ENVIRONMENTS_FIELD_NUMBER: _ClassVar[int]
    environments: _containers.RepeatedCompositeFieldContainer[EnvironmentStep]
    def __init__(self, environments: _Optional[_Iterable[_Union[EnvironmentStep, _Mapping]]] = ...) -> None: ...

class StateUpdate(_message.Message):
    __slots__ = ("reset", "step", "status")
    RESET_FIELD_NUMBER: _ClassVar[int]
    STEP_FIELD_NUMBER: _ClassVar[int]
    STATUS_FIELD_NUMBER: _ClassVar[int]
    reset: Reset
    step: Step
    status: CommunicatorStatus
    def __init__(self, reset: _Optional[_Union[Reset, _Mapping]] = ..., step: _Optional[_Union[Step, _Mapping]] = ..., status: _Optional[_Union[CommunicatorStatus, str]] = ...) -> None: ...
