# vim: set noet:
LIBS=-lgc -ldl

all: rain dyn.so

dyn.so: dyn.c
	clang -shared -fPIC -o $@ $^

rain: main.o core.o vm.o instr.o
	clang -o $@ ${LIBS} $^

main.o: main.c
	clang -o $@ -c $^

instr.o: instr.c
	clang -o $@ -c $^

core.o: core.c
	clang -o $@ -c $^

vm.o: vm.c
	clang -o $@ -c $^

clean:
	rm -rf rain *.o
