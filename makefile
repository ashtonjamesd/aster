CC = gcc
EXEC = a
CFLAGS = -Wextra -Wall
SRCS = $(wildcard src/*.c)

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC)