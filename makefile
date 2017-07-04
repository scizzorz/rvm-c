# vim: set noet:
LIBS=-lgc

all: rain

rain: main.o
	clang -o $@ ${LIBS} $^

main.o: main.c
	clang -o $@ -c $^

clean:
	rm -rf rain *.o
