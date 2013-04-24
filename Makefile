CC=gcc
CFLAGS=$(shell xeno-config --skin=native --cflags)
LFLAGS=$(shell xeno-config --skin=native --ldflags)

beep.o: beep.c
	$(CC) -c beep.c $(CFLAGS)

beep: beep.o
	$(CC) -o beep beep.o $(LFLAGS)

clean:
	  rm -rf *~ *.o beep
