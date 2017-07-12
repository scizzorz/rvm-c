# vim: set noet:
LIBS=-lgc
FLAGS=
EXECS=rain dis step
ARTIFACTS=*.o *.rnc
OBJS=core.o vm.o instr.o table.o builtins.o

all: ${EXECS}

rain: ${OBJS} main.o
	clang ${FLAGS} -o $@ ${LIBS} $^

step: ${OBJS} step.o
	clang ${FLAGS} -o $@ ${LIBS} $^

dis: ${OBJS} dis.o
	clang ${FLAGS} -o $@ ${LIBS} $^

%.o: %.c
	clang ${FLAGS} -o $@ -c $^

clean:
	rm -rf ${EXECS} ${ARTIFACTS}
