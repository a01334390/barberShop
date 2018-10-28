CC = cc
CFLAGS = -lpthread -lm
OBJ = barber.o

%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)

barber : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -f *.o
	rm -f barber