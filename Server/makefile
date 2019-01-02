CC = g++
CFLAGS = -lpthread

default: server

server: main.cpp server.o
	$(CC) $(CFLAGS) -o server main.cpp server.o

server.o: server.cpp server.h
	$(CC) $(CFLAGS) -c server.cpp

clean:
	$(RM) server *.o *~

