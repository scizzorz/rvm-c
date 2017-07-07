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
  _fields_ = [('op', ct.c_uint8),
              ('arg0', ct.c_uint8),
              ('arg1', ct.c_uint8),
              ('arg2', ct.c_uint8)]

  PUSH_CONST = 0x00
  PRINT_ITEM = 0x01
  UN_OP      = 0x02
  BIN_OP     = 0x03
  CMP        = 0x04
  JUMP       = 0x05
  JUMPIF     = 0x06
  DUP        = 0x07
  POP        = 0x08

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


PUSH_CONST = lambda x: Op(Op.PUSH_CONST, x, 0, 0)
PRINT_ITEM = Op(Op.PRINT_ITEM, 0, 0, 0)
ADD = Op(Op.BIN_OP, Op.BIN_ADD, 0, 0)
SUB = Op(Op.BIN_OP, Op.BIN_SUB, 0, 0)
MUL = Op(Op.BIN_OP, Op.BIN_MUL, 0, 0)
DIV = Op(Op.BIN_OP, Op.BIN_DIV, 0, 0)
NEG = Op(Op.UN_OP, Op.UN_NEG, 0, 0)
NOT = Op(Op.UN_OP, Op.UN_NOT, 0, 0)
LT  = Op(Op.CMP, Op.CMP_LT, 0, 0)
GT  = Op(Op.CMP, Op.CMP_GT, 0, 0)
LE  = Op(Op.CMP, Op.CMP_LE, 0, 0)
GE  = Op(Op.CMP, Op.CMP_GE, 0, 0)
EQ  = Op(Op.CMP, Op.CMP_EQ, 0, 0)
NE  = Op(Op.CMP, Op.CMP_NE, 0, 0)
JUMP = lambda pos, neg: Op(Op.JUMP, pos, neg, 0)
JUMPIF = lambda pos, neg: Op(Op.JUMPIF, pos, neg, 0)
DUP = Op(Op.DUP, 0, 0, 0)
POP = Op(Op.POP, 0, 0, 0)



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
  Box.to_rain(0),
  Box.to_rain(1),
  Box.to_rain(10),
  Box.to_rain("LOL"),
]

# PUSH 0 (0)
# PUSH 1 (1)
# ADD
# DUP
# PUSH 2 (10)
# NE
# JUMPIF 0 -5

instrs = [
  PUSH_CONST(0),
  PUSH_CONST(1),
  ADD,
  PRINT_ITEM,
  DUP,
  PUSH_CONST(2),
  NE,
  JUMPIF(0, 7),
  PRINT_ITEM,
]

f = Module('main', consts=consts, instrs=instrs)
f.write()
