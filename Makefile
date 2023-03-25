CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall

all: router server client

router: router.o
	$(CC) -o router router.o

router.o: router.c
	$(CC) -o router.o -c $(CFLAGS) $(CPPFLAGS) router.c

server: server.o
	$(CC) -o server server.o

server.o: server.c
	$(CC) -o server.o -c $(CFLAGS) $(CPPFLAGS) server.c

client: client.o
	$(CC) -o client client.o

client.o: client.c
	$(CC) -o client.o -c $(CFLAGS) $(CPPFLAGS) client.c

clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f router
	@rm -f server
	@rm -f client