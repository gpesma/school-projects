#
# Makefile for the UM Test lab
# 
CC = gcc


IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -l40locality -lcii40-O2 -lm 

EXECS   = um writetests

all: $(EXECS)

um: um.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)


# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

