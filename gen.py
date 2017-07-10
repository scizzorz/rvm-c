import ctypes as ct
import struct

class Box(ct.Structure):
  null = 0
  int = 1
  float = 2
  bool = 3
  str = 4
  table = 5
  func = 6
  cdata = 7

  _saves_ = []
  _strings_ = []

  def __str__(self):
    env_p = ct.cast(self.env, ct.c_void_p).value or 0
    return 'Box({}, {}, 0x{:08x}, 0x{:08x})'.format(self.type, self.size, self.data, env_p)

  def __repr__(self):
    return '<{!s}>'.format(self)

  @classmethod
  def new(cls, *args, **kwargs):
    obj = cls(*args, **kwargs)
    cls._saves_.append(obj)
    return obj

  @classmethod
  def to_rain(cls, val):
    if val is None:
      return cls.new(cls.null, 0, 0, cls.null)
    elif val is False:
      return cls.new(cls.bool, 0, 0, cls.null)
    elif val is True:
      return cls.new(cls.bool, 0, 1, cls.null)
    elif isinstance(val, int):
      return cls.new(cls.int, 0, val, cls.null)
    elif isinstance(val, float):
      raw = struct.pack('d', val)
      intrep = struct.unpack('Q', raw)[0]
      return cls.new(cls.float, 0, intrep, cls.null)
    elif isinstance(val, str):
      idx = len(cls._strings_)
      cls._strings_.append(val)
      return cls.new(cls.str, len(val), idx, cls.null)

    raise Exception("Can't convert value {!r} to Rain".format(val))

Box._fields_ = [('type', ct.c_uint8),
                ('size', ct.c_uint32),
                ('data', ct.c_uint64),
                ('env', ct.POINTER(Box))]
Box.null = ct.POINTER(Box)()

class Op(ct.Structure):
  _fields_ = [('a', ct.c_uint8),
              ('b', ct.c_uint8),
              ('c', ct.c_uint8),
              ('op', ct.c_uint8)]

  PUSH_CONST = 0x00
  PRINT_ITEM = 0x01
  UN_OP      = 0x02
  BIN_OP     = 0x03
  CMP        = 0x04
  JUMP       = 0x05
  JUMPIF     = 0x06
  DUP        = 0x07
  POP        = 0x08
  SET        = 0x09
  GET        = 0x0A
  PUSH_TABLE = 0x0B
  PUSH_SCOPE = 0x0C
  NEW_SCOPE  = 0x0D

  CMP_LT     = 0x00
  CMP_LE     = 0x01
  CMP_GT     = 0x02
  CMP_GE     = 0x03
  CMP_EQ     = 0x04
  CMP_NE     = 0x05

  BIN_ADD    = 0x00
  BIN_SUB    = 0x01
  BIN_MUL    = 0x02
  BIN_DIV    = 0x03

  UN_NEG     = 0x00
  UN_NOT     = 0x01


def si2abc(val):
  b = struct.pack('<i', val)
  return b[0], b[1], b[2]


def ui2abc(val):
  b = struct.pack('<I', val)
  return b[0], b[1], b[2]


PUSH_CONST = lambda x: Op(Op.PUSH_CONST, *ui2abc(x))
PRINT_ITEM = Op(Op.PRINT_ITEM, 0, 0, 0)
ADD = Op(Op.BIN_OP, *ui2abc(Op.BIN_ADD))
SUB = Op(Op.BIN_OP, *ui2abc(Op.BIN_SUB))
MUL = Op(Op.BIN_OP, *ui2abc(Op.BIN_MUL))
DIV = Op(Op.BIN_OP, *ui2abc(Op.BIN_DIV))
NEG = Op(Op.UN_OP, *ui2abc(Op.UN_NEG))
NOT = Op(Op.UN_OP, *ui2abc(Op.UN_NOT))
LT  = Op(Op.CMP, *ui2abc(Op.CMP_LT))
GT  = Op(Op.CMP, *ui2abc(Op.CMP_GT))
LE  = Op(Op.CMP, *ui2abc(Op.CMP_LE))
GE  = Op(Op.CMP, *ui2abc(Op.CMP_GE))
EQ  = Op(Op.CMP, *ui2abc(Op.CMP_EQ))
NE  = Op(Op.CMP, *ui2abc(Op.CMP_NE))
JUMP = lambda offset: Op(Op.JUMP, *si2abc(offset))
JUMPIF = lambda offset: Op(Op.JUMPIF, *si2abc(offset))
DUP = Op(Op.DUP, 0, 0, 0)
POP = Op(Op.POP, 0, 0, 0)
SET = Op(Op.SET, 0, 0, 0)
GET = Op(Op.GET, 0, 0, 0)
PUSH_TABLE = Op(Op.PUSH_TABLE, 0, 0, 0)
PUSH_SCOPE = lambda idx: Op(Op.PUSH_SCOPE, *ui2abc(idx))
NEW_SCOPE = Op(Op.NEW_SCOPE, 0, 0, 0)

class Module:
  def __init__(self, name, consts=[], instrs=[]):
    self.name = name
    self.consts = consts
    self.instrs = instrs

  def write(self):
    with open('{0.name}.rnc'.format(self), 'wb') as fp:
      fp.write(struct.pack('<I', len(self.consts)))
      fp.write(struct.pack('<I', len(self.instrs)))
      fp.write(struct.pack('<I', len(Box._strings_)))

      for string in Box._strings_:
        fp.write(struct.pack('<I', len(string)))
        fp.write(string.encode('utf-8'))

      for const in self.consts:
        fp.write(const)

      for instr in self.instrs:
        fp.write(instr)

consts = [
  Box.to_rain("LOL"),
  Box.to_rain(10),
  Box.to_rain(1),
]

instrs = [
  NEW_SCOPE,
  PUSH_CONST(1),
  PUSH_CONST(0),
  PUSH_SCOPE(0),
  SET,
  PUSH_CONST(0),
  PUSH_SCOPE(0),
  GET,
  PRINT_ITEM,
]

f = Module('main', consts=consts, instrs=instrs)
f.write()
