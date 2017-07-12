# vim: set noet:
LIBS=-lgc
FLAGS=
EXECS=rain dis step
ARTIFACTS=*.o

all: ${EXECS}

rain: main.o core.o vm.o instr.o table.o
	clang ${FLAGS} -o $@ ${LIBS} $^

step: step.o core.o vm.o instr.o table.o
	clang ${FLAGS} -o $@ ${LIBS} $^

dis: dis.o core.o vm.o instr.o table.o
	clang ${FLAGS} -o $@ ${LIBS} $^

main.o: main.c
	clang ${FLAGS} -o $@ -c $^

dis.o: dis.c
	clang ${FLAGS} -o $@ -c $^

step.o: step.c
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
	rm -rf ${EXECS} ${ARTIFACTS}
