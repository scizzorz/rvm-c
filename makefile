# vim: set noet:
LIBS=-lgc
FLAGS=

all: rain

rain: main.o core.o vm.o instr.o table.o
	clang ${FLAGS} -o $@ ${LIBS} $^

main.o: main.c
	clang ${FLAGS} -o $@ -c $^

instr.o: instr.c
	clang ${FLAGS} -o $@ -c $^

core.o: core.c
	clang ${FLAGS} -o $@ -c $^

table.o: table.c
	clang ${FLAGS} -o $@ -c $^

vm.o: vm.c
	clang ${FLAGS} -o $@ -c $^

clean:
	rm -rf rain *.o
