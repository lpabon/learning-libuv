
LIBUVLIBS=$(shell pkg-config --libs libuv)
CC=gcc

all: yahoo hello

hello: hello.c
	$(CC) -o hello hello.c $(LIBUVLIBS)

yahoo: yahoo.c
	$(CC) -o yahoo yahoo.c $(LIBUVLIBS)

clean:
	@rm -f yahoo hello
