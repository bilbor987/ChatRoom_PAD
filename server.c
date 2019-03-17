/**
TCP/IP connection-oriented server-client chat application using C sockets

FD_CLR(fd, &fdset)  
Clears the bit for the file descriptor fd in the file descriptor set fdset.

FD_ISSET(fd, &fdset)  
Returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor set pointed to by fdset, and 0 otherwise.

FD_SET(fd, &fdset)  
Sets the bit for the file descriptor fd in the file descriptor set fdset.

FD_ZERO(&fdset)  
Initializes the file descriptor set fdset to have zero bits for all file descriptors.

*/

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include "common.h"	

void send_to_all(int j, int i, int sockfd, int length, struct MessageHeader recv_buf, fd_set *master) {
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, recv_buf.msg, length, 0) == -1) {
				perror("Didn't send anything");
			}
		}
	}
}
		
void send_message(int i, fd_set *master, int sockfd, int fdmax) {
	int length, j;
	//char recv_buf[BUFSIZE];
	struct MessageHeader recv_buf;
	
	if ((length = recv(i, recv_buf.msg, BUFSIZE, 0)) <= 0) {
		if (length == 0) {
			printf("socket %d hung up\n", i);
		}else {
			perror("Recv error");
		}
		close(i);
		FD_CLR(i, master); //clears the associated bit for the "i" descriptor in the "master" fd_set
	}else { 
		for(j = 0; j <= fdmax; j++){
			send_to_all(j, i, sockfd, length, recv_buf, master );
		}
	}	
}
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr) {
	socklen_t addrlen;
	int newsockfd;
	
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("Acceptance failed");
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
}
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr) {
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("SOCKet not found");
	}
		
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { //god bless GeeksForGeeks for guidance
		perror("Setsockopt error");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) { //10 clients TOPS
		perror("Deaf...");
		exit(1);
	}

	printf("\nTCPServer Waiting for client on port %d\n", PORT);
	fflush(stdout);
}

int main() {
	fd_set master; 
	fd_set read_fds;
	int fdmax, i, sockfd = 0;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	
	FD_ZERO(&master); //initializing the socket sets
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr);
	FD_SET(sockfd, &master); //after esablishing connection, sets the sockfd accordingly in the master set
	
	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("Select failed");
			exit(4);
		}
		
		for (i = 0; i <= fdmax; i++){ //iterate through the set of fds
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd) //if the current one is the one with the connection accept otherwise send message
					connection_accept(&master, &fdmax, sockfd, &client_addr);
				else
					send_message(i, &master, sockfd, fdmax);
			}
		}
	}
	return 0;
}