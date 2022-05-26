CC = g++
CFLAGS = -std=c++17 -g
files := $(wildcard src/*.cpp)
headers := $(files:.cpp=.hpp)
TARGET = sim

all: $(files) $(headers)
	$(CC) $(CFLAGS) -o $(TARGET) $(files) $(ld)

run: all
	./sim