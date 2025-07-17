CC = gcc
EXEC = build/aster
CFLAGS = -Wextra -Wall
SRCS = $(wildcard src/*.c)

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC)