CC = gcc
CFLAGS = -g -Wall
all: echo_client echo_server tiny tiny_client

t.o: t.c t.h
	$(CC) $(CFLAGS) -c t.c

rio.o: rio.c rio.h
	$(CC) $(CFLAGS) -c rio.c

echo_client.o: echo_client.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_client.c

echo_server.o: echo_server.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_server.c

tiny_client.o: tiny_client.c t.h rio.h
	$(CC) $(CFLAGS) -c tiny_client.c

tiny_client: tiny_client.o t.o rio.o
	$(CC) $(CFLAGS) tiny_client.o t.o rio.o -o tiny_client

echo_client: echo_client.o t.o rio.o
	$(CC) $(CFLAGS) echo_client.o t.o rio.o -o echo_client 

echo_server: echo_server.o t.o rio.o
	$(CC) $(CFLAGS) echo_server.o t.o rio.o -o echo_server

tiny.o: tiny.c t.h rio.h
	$(CC) $(CFLAGS) -c tiny.c

tiny: tiny.o t.o rio.o
	$(CC) $(CFLAGS) tiny.o t.o rio.o -o tiny

clean:
	rm -f *.o echo_client adder echo_server tiny tiny_client
