# vim: set noet:
LIBS=-lgc -ldl
FLAGS=
EXECS=rain dis step builtins.so
ARTIFACTS=*.o *.rnc
OBJS=core.o vm.o instr.o table.o

all: ${EXECS}

rain: librain.so main.o
	clang ${FLAGS} -o $@ ${LIBS} $^

step: librain.so step.o
	clang ${FLAGS} -o $@ ${LIBS} $^

dis: librain.so dis.o
	clang ${FLAGS} -o $@ ${LIBS} $^

main.o: main.c
	clang ${FLAGS} -o $@ -c $^

dis.o: dis.c
	clang ${FLAGS} -o $@ -c $^

step.o: step.c
	clang ${FLAGS} -o $@ -c $^

builtins.so: builtins.c
	clang ${FLAGS} -fPIC -shared -o $@ $^

librain.so: core.c vm.c instr.c table.c
	clang ${FLAGS} -fPIC -shared -o $@ $^

clean:
	rm -rf ${EXECS} ${ARTIFACTS}
