import rvmpy

main = rvmpy.Module('main')

_builtins = main.add_const('/home/john/rainvm/builtins.so')
_print = main.add_const('R_builtin_print')
_printn = main.add_const('print')
hello = main.add_const('HELLO')

with main.goto(main.main):
  main.push_const(_builtins)
  main.push_const(_print)
  main.load()
  main.push_const(_printn)
  main.push_scope()
  main.set()

  main.push_const(hello)
  main.push_const(_printn)
  main.push_scope()
  main.get()
  main.call()

main.write()
