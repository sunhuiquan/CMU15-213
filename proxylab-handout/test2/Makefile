# note that varibles need use = and then be used by $()
CC =  gcc
CFLAGS =  -g -Wall
all: echo_server_2 echo_client echo_server_3

t.o: t.c t.h
	$(CC) $(CFLAGS) -c t.c

rio.o: rio.c rio.h
	$(CC) $(CFLAGS) -c rio.c

echo_server_2.o: echo_server_2.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_server_2.c

echo_server_3.o: echo_server_3.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_server_3.c

echo_client.o: echo_client.c t.h rio.h
	$(CC) $(CFLAGS) -c echo_client.c

echo_server_2: echo_server_2.o t.o rio.o
	$(CC) $(CFLAGS) echo_server_2.o t.o rio.o -o echo_server_2

echo_server_3: echo_server_3.o t.o rio.o
	$(CC) $(CFLAGS) echo_server_3.o t.o rio.o -o echo_server_3 -pthread

echo_client: echo_client.o t.o rio.o
	$(CC) $(CFLAGS) echo_client.o t.o rio.o -o echo_client

clean:
	rm *.o echo_server_2

