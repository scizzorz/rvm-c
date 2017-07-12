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


class BinOp(Ux):
  op = Instr.BIN_OP

  BIN_ADD    = 0x00
  BIN_SUB    = 0x01
  BIN_MUL    = 0x02
  BIN_DIV    = 0x03


class UnOp(Ux):
  op = Instr.UN_OP

  UN_NEG     = 0x00
  UN_NOT     = 0x01


class Cmp(Ux):
  op = Instr.CMP

  CMP_LT     = 0x00
  CMP_LE     = 0x01
  CMP_GT     = 0x02
  CMP_GE     = 0x03
  CMP_EQ     = 0x04
  CMP_NE     = 0x05


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

  def add_const(self, val):
    if self.frozen:
      raise Exception('Module {!r} already finalized'.format(self.name))

    self.consts.append(Box.to_rain(val))
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

  def ret(self):
    self.add_instr(Return())

  def imp(self):
    self.add_instr(Import())

  def add(self):
    self.add_instr(BinOp(BinOp.BIN_ADD))

  def sub(self):
    self.add_instr(BinOp(BinOp.BIN_SUB))

  def mul(self):
    self.add_instr(BinOp(BinOp.BIN_MUL))

  def div(self):
    self.add_instr(BinOp(BinOp.BIN_DIV))

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


main = Module('main')

fn_test = main.add_block()

with main.goto(main.main):
  main.call_to(fn_test)
  main.ret()

with main.goto(fn_test):
  i3 = main.add_const(3)
  i4 = main.add_const(4)

  main.push_const(i3)
  main.push_const(i4)
  main.add()
  main.print()
  main.ret()

main.write()

iftest = Module('if')

false_br = iftest.add_block()
true_br = iftest.add_block()
next_br = iftest.add_block()

with iftest.goto(iftest.main):
  const = iftest.add_const(False)
  iftest.push_const(const)
  iftest.jump_if(true_br)
  iftest.jump(false_br)

with iftest.goto(false_br):
  text = iftest.add_const('False!')
  iftest.push_const(text)
  iftest.print()
  iftest.jump(next_br)

with iftest.goto(true_br):
  text = iftest.add_const('True!')
  iftest.push_const(text)
  iftest.print()
  iftest.jump(next_br)

with iftest.goto(next_br):
  text = iftest.add_const('Done!')
  iftest.push_const(text)
  iftest.print()

iftest.write()


looptest = Module('loop')

neg = looptest.add_const(-1)
end = looptest.add_const(0)
start = looptest.add_const(10)
name = looptest.add_const('x')

loop_body = looptest.add_block()
loop_before = looptest.add_block()
loop_after = looptest.add_block()

with looptest.goto(looptest.main):
  looptest.push_const(start)
  looptest.push_const(name)
  looptest.push_scope()
  looptest.set()

  looptest.jump(loop_before)

with looptest.goto(loop_before):
  looptest.push_const(end)
  looptest.push_const(name)
  looptest.push_scope()
  looptest.get()

  looptest.add_instr(Cmp(Cmp.CMP_GT))
  looptest.jump_if(loop_body)
  looptest.jump(loop_after)

with looptest.goto(loop_body):
  looptest.push_const(name)
  looptest.push_scope()
  looptest.get()
  looptest.push_const(neg)
  looptest.add()
  looptest.dup()
  looptest.print()
  looptest.push_const(name)
  looptest.push_scope()
  looptest.set()

  looptest.jump(loop_before)

with looptest.goto(loop_after):
  done = looptest.add_const('Done!')
  looptest.push_const(done)
  looptest.print()

looptest.write()


test = Module('test')

with test.goto(test.main):
  hello = test.add_const('Hello, world!')
  main_f = test.add_const('main.rnc')

  test.push_const(hello)
  test.print()
  test.push_const(main_f)
  test.imp()
  test.ret()

test.write()
