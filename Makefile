CC      = cc
CFLAGS  = -O2 -g -fcommon -I.
LDFLAGS = 

OBJS = args.o builtin.o cmd.o ctype.o error.o expand.o fault.o io.o macro.o main.o msg.o name.o print.o profile.o service.o shims.o stak.o string.o test.o word.o xec.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o sh $(OBJS) $(LDFLAGS)

clean:
	rm -f sh $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
