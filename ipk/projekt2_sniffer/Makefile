CC = g++
LD = -lpcap
CFLAGS = -std=c++17
files := $(wildcard *.cpp)
headers := $(files:.cpp=.hpp)
TARGET = ipk-sniffer

all: $(files) $(headers)
	$(CC) $(CFLAGS) -o $(TARGET) $(files) $(LD)