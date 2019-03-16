all:
	@gcc -Wall -o client client.c
	@gcc -Wall -o server server.c

server:
	@gcc -Wall -o server server.c

client:
	@gcc -Wall -o client client.c

clean:
	@rm client server