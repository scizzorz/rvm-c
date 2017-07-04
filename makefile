# vim: set noet:
LIBS=-lgc

all: rain

rain: main.o rain.o
	clang -o $@ ${LIBS} $^

main.o: main.c
	clang -o $@ -c $^

rain.o: rain.c
	clang -o $@ -c $^

clean:
	rm -rf rain *.o
