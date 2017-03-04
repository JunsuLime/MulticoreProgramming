CC = g++
CXXFLAGS = -g -Wall -std=c++11 -Os

# Macros specifying path for compile.
TARGET = path_finder

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
BIN = ./bin/
INC = ./include/
SRC = ./src/
LIB = ./lib/ -lpthread

# pre-processor.
CPPFLAGS += -I$(INC) -lpthread

# compile command
TARGET: $(OBJS)
	$(CC) $(CXXFLAGS) -o  $(BIN)$(TARGET) $(OBJS) -L$(LIB)

# Delete binary & object files.
clean:
	rm $(BIN)$(TARGET) $(OBJS)

