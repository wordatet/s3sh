# Makefile for s3sh (System III Shell) - Hybrid Port
# Supports GCC for stability and PCC for historical auditing.

CC      ?= gcc
CFLAGS  += -g -O0 -D_GNU_SOURCE -I. -I./include -fcommon -fno-stack-protector
LDFLAGS += -g

OBJS = args.o builtin.o cmd.o ctype.o error.o expand.o fault.o io.o macro.o \
       main.o msg.o name.o print.o profile.o service.o shims.o stak.o \
       string.o test.o word.o xec.o

PROG = sh

.PHONY: all clean vintage paranoid

# Default: GCC build
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Vintage: PCC build
vintage: clean
	$(MAKE) CC=pcc CFLAGS="-g -O0 -I. -I./include"

# Verification: Run the Super Paranoid suite
paranoid: clean $(PROG)
	./super_paranoid_s3.sh

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(PROG)
