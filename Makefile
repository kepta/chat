CC = clang
CCFLAGS = -lpthread

build: main.c lib/library.c lib/pa_allocation.c lib/pa_converters.c lib/pa_cpuload.c lib/pa_debugprint.c lib/pa_dither.c
			$(CC) $(CCFLAGS) -o chat main.c lib/library.c
