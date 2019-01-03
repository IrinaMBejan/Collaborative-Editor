CC = g++
CFLAGS = -g -Wall -pthread

default: server

server: main.cpp server.o  utils.o handlerthread.o server.o csv.o
	$(CC) $(CFLAGS) -o server main.cpp server.o utils.o handlerthread.o csv.o

server.o: server.cpp server.h 
	$(CC) $(CFLAGS) -c server.cpp

handlerthread.o: handlerthread.cpp handlerthread.h
	$(CC) $(CFLAGS) -c handlerthread.cpp

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

csv.o: csv.cpp csv.h
	$(CC) $(CFLAGS) -c csv.cpp

clean:
	$(RM) server *.o *~

