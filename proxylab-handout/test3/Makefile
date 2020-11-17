CC = gcc
CFLAGS = -g -Wall -pthread
all: echo_server_4 echo_client

t.o: t.c t.h
	$(CC) $(CFLAGS) -c t.c

rio.o: rio.c rio.h
	$(CC) $(CFLAGS) -c rio.c

sbuf.o: sbuf.c sbuf.h
	$(CC) $(CFLAGS) -c sbuf.c

echo_server_4.o: echo_server_4.c t.h rio.h sbuf.h
	$(CC) $(CFLAGS) -c echo_server_4.c

echo_clien.o: echo_client.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_client.c

echo_server_4: echo_server_4.o t.o rio.o sbuf.o
	$(CC) $(CFLAGS) echo_server_4.o t.o rio.o sbuf.o -o echo_server_4

echo_client: echo_client.o t.o rio.o
	$(CC) $(CFLAGS) echo_client.o t.o rio.o -o echo_client

clean:
	rm -f echo_server_4 echo_client *.o
