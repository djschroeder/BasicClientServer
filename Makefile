# Makefile - written by ChatGPT

# Compiler and flags
CC=gcc
CFLAGS=-I. -Wall -std=c99 -O

# Object files
OBJ_TANDS=tands.o
OBJ_SERVER=server.o $(OBJ_TANDS)
OBJ_CLIENT=client.o $(OBJ_TANDS)

# Executable names
EXEC_SERVER=server
EXEC_CLIENT=client

# Default target
all: $(EXEC_SERVER) $(EXEC_CLIENT)

# Server program
$(EXEC_SERVER): $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

# Client program
$(EXEC_CLIENT): $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up
.PHONY: clean
clean:
	rm -f $(EXEC_SERVER) $(EXEC_CLIENT) *.o
