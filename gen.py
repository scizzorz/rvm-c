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
      str_p = ct.create_string_buffer(val.encode('utf-8'))
      cls._saves_.append(str_p)
      return cls.new(cls.str, len(val), ct.cast(str_p, ct.c_void_p).value, cls.null)

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

  LOAD_CONST = 0

LOAD_CONST = lambda x: Op(Op.LOAD_CONST, x, 0, 0)


class Module:
  def __init__(self, name, consts=[], instrs=[]):
    self.name = name
    self.consts = consts
    self.instrs = instrs

  def write(self):
    with open(f'{self.name}.rnc', 'wb') as fp:
      fp.write(struct.pack('<I', len(self.consts)))
      fp.write(struct.pack('<I', len(self.instrs)))

      for const in self.consts:
        fp.write(const)

      for instr in self.instrs:
        fp.write(instr)

consts = [
  Box.to_rain(10),
  Box.to_rain(12),
]

instrs = [
  LOAD_CONST(0),
  LOAD_CONST(1),
]

f = Module('main', consts=consts, instrs=instrs)
f.write()
