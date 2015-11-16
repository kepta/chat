CC = clang
CCFLAGS = -lpthread

build: main.c library.c
			$(CC) $(CCFLAGS) -o chat main.c library.c
