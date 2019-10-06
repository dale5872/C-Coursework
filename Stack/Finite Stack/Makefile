all: stacktest

CCFLAGS = -Wall -pedantic

stacktest: stack.o stacktest.c
	gcc $(CCFLAGS) -o stacktest stacktest.c stack.o

%.o: %.c %.h
	gcc $(CCFLAGS) -c $<

clean:
	$(RM) stacktest stack.o
