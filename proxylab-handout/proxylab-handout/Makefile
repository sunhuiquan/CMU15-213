CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread
all: proxy

t.o: t.c t.h
	$(CC) $(CFLAGS) -c t.c

rio.o: rio.c rio.h
	$(CC) $(CFLAGS) -c rio.c

proxy.o: proxy.c t.h rio.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o t.o rio.o
	$(CC) $(CFLAGS) proxy.o t.o rio.o -o proxy $(LDFLAGS)

clean:
	rm -f *.o proxy
