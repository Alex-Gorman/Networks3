CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall

all: router server client client2

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

# server2: server2.o
# 	$(CC) -o server2 server2.o

# server2.o: server2.c
# 	$(CC) -o server2.o -c $(CFLAGS) $(CPPFLAGS) server2.c

client2: client2.o
	$(CC) -o client2 client2.o

client2.o: client2.c
	$(CC) -o client2.o -c $(CFLAGS) $(CPPFLAGS) client2.c

clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f router
	@rm -f server
	@rm -f client
	@rm -f client2