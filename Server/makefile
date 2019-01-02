CC = g++
CFLAGS = -pthread

default: server

server: main.cpp server.o handlerthread.o
	$(CC) $(CFLAGS) -o server main.cpp server.o handlerthread.o

server.o: server.cpp server.h
	$(CC) $(CFLAGS) -c server.cpp

handlerthread.o: handlerthread.cpp handlerthread.h
	$(CC) $(CFLAGS) -c handlerthread.cpp

clean:
	$(RM) server *.o *~

