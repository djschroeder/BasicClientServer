CC = gcc
CFLAGS = -Wall -std=c99
TARGET = prodcon
SRCS = prodcon.c logf.c tands.c
OBJS = $(SRCS:.c=.o)
HEADER = header.h

# Default is to compile with debugging
all: CFLAGS += -g
all: $(TARGET)

# For optimized version
opt: CFLAGS += -O
opt: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
