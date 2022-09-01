all: lets-talk

lets-talk: lets-talk.o list.o
	gcc -Wall -o lets-talk lets-talk.o list.o -lpthread

lets-talk.o: lets-talk.c list.h
	gcc -c lets-talk.c -lpthread

list.o: list.c list.h
	gcc -c list.c

clean:
	$(RM) lets-talk myapp list *.o *.exe