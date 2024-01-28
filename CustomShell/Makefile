CC = g++
CFLAGS = -Wall -std=c++11
TARGET = shell379
SRCS = main.cpp utilities.cpp ctrlfuncs.cpp
OBJS = $(SRCS:.cpp=.o)

HEADER = header.hpp

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADER)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)