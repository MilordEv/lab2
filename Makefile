CC=gcc -std=c11 -D_GNU_SOURCE
CFLAGS=-c
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)


all: clean main

main: $(OBJECTS)
	./progA.o

.c.o:
	$(CC) $< -o $@

clean:
	rm -rf *.o *.exe main *.a tests/*.o test *.gcov *.gcda *.gcno *.info report

check_style:
	clang-format -style=google -n *.c

make_style:
	clang-format -style=google -i *.c

rebuild: clean all