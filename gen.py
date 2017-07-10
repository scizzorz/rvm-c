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
      return cls.new(cls.null, 0, 0, cls.nullptr)
    elif val is False:
      return cls.new(cls.bool, 0, 0, cls.nullptr)
    elif val is True:
      return cls.new(cls.bool, 0, 1, cls.nullptr)
    elif isinstance(val, int):
      return cls.new(cls.int, 0, val, cls.nullptr)
    elif isinstance(val, float):
      raw = struct.pack('d', val)
      intrep = struct.unpack('Q', raw)[0]
      return cls.new(cls.float, 0, intrep, cls.nullptr)
    elif isinstance(val, str):
      idx = len(cls._strings_)
      cls._strings_.append(val)
      return cls.new(cls.str, len(val), idx, cls.nullptr)

    raise Exception("Can't convert value {!r} to Rain".format(val))


Box._fields_ = [('type', ct.c_uint8),
                ('size', ct.c_uint32),
                ('data', ct.c_uint64),
                ('env', ct.POINTER(Box))]
Box.nullptr = ct.POINTER(Box)()


class COp(ct.Structure):
  _fields_ = [('a', ct.c_uint8),
              ('b', ct.c_uint8),
              ('c', ct.c_uint8),
              ('op', ct.c_uint8)]

  PUSH_CONST = 0x00
  PRINT      = 0x01
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


class Op(COp):
  op = 0xFF

  def __init__(self, a, b, c):
    super().__init__(self.op, a, b, c)

class Nx(Op):
  def __init__(self):
    super().__init__(0, 0, 0)

class Sx(Op):
  def __init__(self, x):
    a, b, c, *_ = struct.pack('<i', x)
    super().__init__(a, b, c)

class Sxyz(Op):
  def __init__(self, x, y, z):
    a, b, c = map(lambda x: struct.pack('<b', x), (x, y, z))
    super().__init__(a, b, c)

class Ux(Op):
  def __init__(self, x):
    a, b, c, *_ = struct.pack('<I', x)
    super().__init__(a, b, c)

class Uxyz(Op):
  def __init__(self, x, y, z):
    a, b, c = map(lambda x: struct.pack('<B', x), (x, y, z))
    super().__init__(a, b, c)

class PushConst(Ux): op = COp.PUSH_CONST
class Print(Nx): op = COp.PRINT
class BinOp(Ux): op = COp.BIN_OP
class UnOp(Ux): op = COp.UN_OP
class Cmp(Ux): op = COp.CMP
class Jump(Sx): op = COp.JUMP
class JumpIf(Sx): op = COp.JUMPIF
class Dup(Nx): op = COp.DUP
class Pop(Nx): op = COp.POP
class Set(Nx): op = COp.SET
class Get(Nx): op = COp.GET
class PushTable(Nx): op = COp.PUSH_TABLE
class PushScope(Ux): op = COp.PUSH_SCOPE
class NewScope(Nx): op = COp.NEW_SCOPE

class Module:
  def __init__(self, name, consts=[], instrs=[]):
    self.name = name
    self.consts = consts
    self.instrs = instrs

  def add_const(self, val):
    self.consts.append(Box.to_rain(val))
    return len(self.consts) - 1

  def add_instr(self, *instrs):
    self.instrs.extend(instrs)

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

mod = Module('main')

x = mod.add_const('x')
y = mod.add_const('y')
z = mod.add_const('z')
i3 = mod.add_const(3)
i4 = mod.add_const(4)

mod.add_instr(PushConst(i3),
              PushConst(x),
              PushScope(0),
              Set(),
              PushConst(i4),
              PushConst(y),
              PushScope(0),
              Set(),
              PushConst(x),
              PushScope(0),
              Get(),
              PushConst(y),
              PushScope(0),
              Get(),
              BinOp(COp.BIN_ADD),
              PushConst(z),
              PushScope(0),
              Set(),
              PushConst(z),
              PushScope(0),
              Get(),
              Dup(),
              BinOp(COp.BIN_ADD),
              Print(),
              )

mod.write()
