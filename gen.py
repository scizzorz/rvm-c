import rvmpy

main = rvmpy.Module('main')

_builtins = main.add_const('/home/john/rainvm/builtins.so')
_print = main.add_const('R_builtin_print')
_printn = main.add_const('print')

load = main.add_const('load')
hello = main.add_const('hello')

with main.goto(main.main):
  main.push_const(_builtins)
  main.push_const(_print)
  main.push_const(load)
  main.push_scope()
  main.get()
  main.call()
  main.push_const(_printn)
  main.push_scope()
  main.set()

  main.push_const(hello)
  main.push_const(_printn)
  main.push_scope()
  main.get()
  main.call()

main.write()
