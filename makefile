# vim: set noet:
LIBS=-lgc -ldl
FLAGS=
EXECS=rain dis step builtins.so
ARTIFACTS=*.o *.rnc
OBJS=core.o vm.o instr.o table.o

all: ${EXECS}

rain: ${OBJS} main.o
	clang ${FLAGS} -o $@ ${LIBS} $^

step: ${OBJS} step.o
	clang ${FLAGS} -o $@ ${LIBS} $^

dis: ${OBJS} dis.o
	clang ${FLAGS} -o $@ ${LIBS} $^

builtins.so: builtins.c
	clang ${FLAGS} -fPIC -shared -o $@ $^

%.o: %.c
	clang ${FLAGS} -o $@ -c $^

clean:
	rm -rf ${EXECS} ${ARTIFACTS}
