from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from typing import ClassVar as _ClassVar

DESCRIPTOR: _descriptor.FileDescriptor

class DType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = ()
    FLOAT32: _ClassVar[DType]
    UINT8: _ClassVar[DType]
    UINT16: _ClassVar[DType]
    UINT32: _ClassVar[DType]
    UINT64: _ClassVar[DType]
    INT8: _ClassVar[DType]
    INT16: _ClassVar[DType]
    INT32: _ClassVar[DType]
    INT64: _ClassVar[DType]
    FLOAT16: _ClassVar[DType]
    FLOAT64: _ClassVar[DType]
    BOOL: _ClassVar[DType]
FLOAT32: DType
UINT8: DType
UINT16: DType
UINT32: DType
UINT64: DType
INT8: DType
INT16: DType
INT32: DType
INT64: DType
FLOAT16: DType
FLOAT64: DType
BOOL: DType
