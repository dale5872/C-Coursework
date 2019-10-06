all: infstacktest

CCFLAGS = -Wall -pedantic

infstacktest: infstack.o infstacktest.c
	gcc $(CCFLAGS) -o infstacktest infstacktest.c infstack.o

%.o: %.c %.h
	gcc $(CCFLAGS) -c $<

clean:
	$(RM) infstacktest infstack.o
