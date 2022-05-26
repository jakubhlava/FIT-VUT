CC = g++
CFLAGS = -std=c++17 -g
files := $(wildcard src/*.cpp)
ld := -lstdc++fs
headers := $(files:.cpp=.hpp)
TARGET = client

all: $(files) $(headers)
	$(CC) $(CFLAGS) -o $(TARGET) $(files) $(ld)