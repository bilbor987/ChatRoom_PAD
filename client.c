#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
		
void send_message(int i, int sockfd, int color_choice)
{
	struct MessageHeader send_buf, recv_buf;
	int nbyte_recvd;

	if (i == 0){ //reading state
		fgets(send_buf.msg, BUFSIZE, stdin);
		if (strcmp(send_buf.msg , "quit\n") == 0) { //FANCY way to CTRL+C
			exit(0);
		}else
			//send(sockfd, send_buf.id,)
			send(sockfd, send_buf.msg, strlen(send_buf.msg), 0);
	}else { //writing state
		nbyte_recvd = recv(sockfd, recv_buf.msg, BUFSIZE, 0); //saves the length of the message
		recv_buf.msg[nbyte_recvd] = '\0'; //trims it nicely
		switch(color_choice){
			case 1: printf(ANSI_COLOR_YELLOW "%s\n", recv_buf.msg);break;
			case 2: printf(ANSI_COLOR_MAGENTA "%s\n", recv_buf.msg);break;
			case 3: printf(ANSI_COLOR_CYAN "%s\n", recv_buf.msg);break;
			default: printf("aiurea\n");exit(0);
		}
		printf(ANSI_COLOR_CYAN "%s\n", recv_buf.msg);
		fflush(stdout);
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket not valid");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0', sizeof(server_addr->sin_zero));
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Connect didn't work");
		exit(1);
	}
}
	
int main()
{
	int sockfd, fdmax, i, color_choice;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	printf("\nloc de ales culoarea:\n");
	printf("1.yellow\n2.magenta\n3.cyan\n");
	scanf("%d",&color_choice);
	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);
	fdmax = sockfd;
	
	while(1){

		read_fds = master;
		//printf(ANSI_COLOR_YELLOW "type here: "ANSI_COLOR_RESET);
		//fflush(stdout);
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("Nothing to SELECT bro");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_message(i, sockfd, color_choice);
	}
	printf("Client-quited\n");
	close(sockfd);
	return 0;
}