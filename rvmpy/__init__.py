from contextlib import contextmanager as ctx
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

  # strings should be saved on Module, not Box
  # right now, every Module produced writes the whole set of strings
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

    elif isinstance(val, type(lambda: 0)):
      return cls.new(cls.func, 0, val(), cls.nullptr)

    raise Exception("Can't convert value {!r} to Rain".format(val))


Box._fields_ = [('type', ct.c_uint8),
                ('size', ct.c_uint32),
                ('data', ct.c_uint64),
                ('env', ct.POINTER(Box))]
Box.nullptr = ct.POINTER(Box)()


class CInstr(ct.Structure):
  _fields_ = [('a', ct.c_uint8),
              ('b', ct.c_uint8),
              ('c', ct.c_uint8),
              ('op', ct.c_uint8)]


class Instr:
  op = 0xFF

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
  NOP        = 0x0D
  CALLTO     = 0x0E
  RETURN     = 0x0F
  IMPORT     = 0x10
  CALL       = 0x11
  SET_META   = 0x12
  GET_META   = 0x13
  LOAD       = 0x14
  SAVE       = 0x15

  def __init__(self):
    pass

  def as_c(self):
    return CInstr(self.op, 0, 0, 0)


class Nx(Instr):
  pass


class Sx(Instr):
  def __init__(self, x):
    self.x = x

  def as_c(self):
    a, b, c, *_ = struct.pack('<i', self.x)
    return CInstr(self.op, a, b, c)


class SBx(Sx):
  def __init__(self, block):
    self.block = block


class Sxyz(Instr):
  def __init__(self, x, y, z):
    self.x = x
    self.y = y
    self.z = z

  def as_c(self):
    a, b, c = map(lambda x: struct.pack('<b', x), (self.x, self.y, self.z))
    return CInstr(self.op, a, b, c)


class Ux(Instr):
  def __init__(self, x):
    self.x = x

  def as_c(self):
    a, b, c, *_ = struct.pack('<I', self.x)
    return CInstr(self.op, a, b, c)


class UBx(Ux):
  def __init__(self, block):
    self.block = block


class Uxyz(Instr):
  def __init__(self, x, y, z):
    self.x = x
    self.y = y
    self.z = z

  def as_c(self):
    a, b, c = map(lambda x: struct.pack('<B', x), (self.x, self.y, self.z))
    return CInstr(self.op, a, b, c)


class PushConst(Ux): op = Instr.PUSH_CONST
class Print(Nx): op = Instr.PRINT
class Jump(SBx): op = Instr.JUMP
class JumpIf(SBx): op = Instr.JUMPIF
class Dup(Nx): op = Instr.DUP
class Pop(Nx): op = Instr.POP
class Set(Nx): op = Instr.SET
class Get(Nx): op = Instr.GET
class PushTable(Nx): op = Instr.PUSH_TABLE
class PushScope(Nx): op = Instr.PUSH_SCOPE
class NOP(Nx): op = Instr.NOP
class CallTo(UBx): op = Instr.CALLTO
class Return(Nx): op = Instr.RETURN
class Import(Nx): op = Instr.IMPORT
class Call(Ux): op = Instr.CALL
class SetMeta(Nx): op = Instr.SET_META
class GetMeta(Nx): op = Instr.GET_META
class Load(Nx): op = Instr.LOAD
class Save(Nx): op = Instr.SAVE


class BinOp(Ux):
  op = Instr.BIN_OP

  ADD    = 0x00
  SUB    = 0x01
  MUL    = 0x02
  DIV    = 0x03


class UnOp(Ux):
  op = Instr.UN_OP

  UN_NEG     = 0x00
  UN_NOT     = 0x01


class CmpOp(Ux):
  op = Instr.CMP

  LT     = 0x00
  LE     = 0x01
  GT     = 0x02
  GE     = 0x03
  EQ     = 0x04
  NE     = 0x05


class Block:
  def __init__(self):
    self.addr = None
    self.instrs = []

  def __len__(self):
    return len(self.instrs)

  def write(self, fp):
    for instr in self.instrs:
      fp.write(instr.as_c())

  def add_instr(self, *instrs):
    self.instrs.extend(instrs)

  def set_addr(self, addr):
    self.addr = addr

  def finalize(self):
    self.instrs = tuple(self.instrs)

    for i, instr in enumerate(self.instrs):
      if isinstance(instr, CallTo):
        instr.x = instr.block.addr

      elif isinstance(instr, (Jump, JumpIf)):
        instr.x = instr.block.addr - (self.addr + i) - 1


class Module:
  def __init__(self, name):
    self.name = name
    self.consts = []
    self.block = None
    self.main = Block()
    self.blocks = [self.main]
    self.frozen = False
    self.instr_count = 0

  def finalize(self):
    # hacky, but works for now
    self.blocks = tuple(self.blocks)
    self.consts = tuple(self.consts)
    self.frozen = True

    for block in self.blocks:
      block.set_addr(self.instr_count)
      self.instr_count += len(block)

    for block in self.blocks:
      block.finalize()

    self.consts = [Box.to_rain(val) for val in self.consts]

  def add_const(self, val):
    if self.frozen:
      raise Exception('Module {!r} already finalized'.format(self.name))

    if val in self.consts:
      return self.consts.index(val)

    self.consts.append(val)
    return len(self.consts) - 1

  def add_instr(self, *instrs):
    if self.frozen:
      raise Exception('Module {!r} already finalized'.format(self.name))

    self.block.add_instr(*instrs)

  def push_const(self, idx):
    self.add_instr(PushConst(idx))

  def push_scope(self):
    self.add_instr(PushScope())

  def push_table(self):
    self.add_instr(PushTable())

  def pop(self):
    self.add_instr(Pop())

  def dup(self):
    self.add_instr(Dup())

  def print(self):
    self.add_instr(Print())

  def set(self):
    self.add_instr(Set())

  def get(self):
    self.add_instr(Get())

  def jump(self, offset):
    self.add_instr(Jump(offset))

  def jump_if(self, offset):
    self.add_instr(JumpIf(offset))

  def call_to(self, instr):
    self.add_instr(CallTo(instr))

  def call(self, argc):
    self.add_instr(Call(argc))

  def set_meta(self):
    self.add_instr(SetMeta())

  def get_meta(self):
    self.add_instr(GetMeta())

  def load(self):
    self.add_instr(Load())

  def ret(self):
    self.add_instr(Return())

  def save(self):
    self.add_instr(Save())

  def imp(self):
    self.add_instr(Import())

  def add(self):
    self.add_instr(BinOp(BinOp.ADD))

  def sub(self):
    self.add_instr(BinOp(BinOp.SUB))

  def mul(self):
    self.add_instr(BinOp(BinOp.MUL))

  def div(self):
    self.add_instr(BinOp(BinOp.DIV))

  def lt(self):
    self.add_instr(CmpOp(CmpOp.LT))

  def le(self):
    self.add_instr(CmpOp(CmpOp.LE))

  def gt(self):
    self.add_instr(CmpOp(CmpOp.GT))

  def ge(self):
    self.add_instr(CmpOp(CmpOp.GE))

  def eq(self):
    self.add_instr(CmpOp(CmpOp.EQ))

  def ne(self):
    self.add_instr(CmpOp(CmpOp.NE))

  def nop(self):
    self.add_instr(NOP())

  def write(self):
    self.finalize()

    with open('{0.name}.rnc'.format(self), 'wb') as fp:
      fp.write(struct.pack('<I', len(self.consts)))
      fp.write(struct.pack('<I', self.instr_count))
      fp.write(struct.pack('<I', len(Box._strings_)))

      for string in Box._strings_:
        fp.write(struct.pack('<I', len(string)))
        fp.write(string.encode('utf-8'))

      for const in self.consts:
        fp.write(const)

      for block in self.blocks:
        block.write(fp)

  def add_block(self):
    block = Block()
    self.blocks.append(block)
    return block

  @ctx
  def goto(self, block):
    temp = self.block
    self.block = block
    yield
    self.block = temp
