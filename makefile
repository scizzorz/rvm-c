# vim: set noet:
LIBS=-lgc -ldl
EXECS=rain dis step
LIB=librain.so
LIB_OBJS=core.o vm.o instr.o table.o builtins.o

all: $(LIB) $(EXECS)

$(LIB): $(LIB_OBJS)
	clang $(FLAGS) -fPIC -shared -o $@ $^

$(LIB_OBJS): %.o: %.c
	clang $(FLAGS) -fPIC -c -o $@ $^

$(EXECS): %: %.o $(LIB)
	clang $(FLAGS) $(LIBS) -o $@ $^

%.o: %.c
	clang $(FLAGS) -c -o $@ $^

clean:
	rm -rf $(EXECS) $(LIB) *.o
