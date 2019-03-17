all:
	@gcc -Wall -o client client.c -lncurses
	@gcc -Wall -o server server.c -lncurses

server:
	@gcc -Wall -o server server.c -lncurses

client:
	@gcc -Wall -o client client.c -lncurses

clean:
	@rm client server