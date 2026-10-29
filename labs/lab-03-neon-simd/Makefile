CC = gcc
CFLAGS = -O3 -Wall -Wextra

all: neon_benchmark

neon_benchmark: neon_benchmark.c
	$(CC) $(CFLAGS) neon_benchmark.c -o neon_benchmark

clean:
	rm -f neon_benchmark