# vim: set noet:
LIBS=-lgc

all: rain

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
