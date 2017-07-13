CC = gcc
CFLAGS = -Wall -g -O3

OBJS = edifactread edifactparse

all: $(OBJS)

edifactread: edifactread.c

edifactparse: edifactparse.c edifact.o

edifact.o: edifact.c


