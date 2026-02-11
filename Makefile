all: indent build test

indent:
	clang-format -i *.h *.c

build: bst

bst: bst.h bst.c bst_tests.c
	gcc -std=c99 -Wall -Wextra -pedantic -g -O2 $^ -o $@

test: bst
	valgrind -q --leak-check=full ./$<

clean:
	rm -f bst *~
