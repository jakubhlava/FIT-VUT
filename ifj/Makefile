CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

files := $(wildcard *.c)
headers := $(files:.c=.h)

TARGET = ifj20c

all: ${files} ${headers}
	${CC} ${CFLAGS} -o ${TARGET} ${files} 