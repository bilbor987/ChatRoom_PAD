#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
		
void send_message(int i, int sockfd, int color_choice, char user[20]){
	struct MessageHeader sender, reciever;
	int length;

	if (i == 0){ //reading state
		fgets(sender.msg, BUFSIZE, stdin);
		if (strcmp(sender.msg , "quit\n") == 0) { //FANCY way to CTRL+C
			exit(0);
		}else
			//send(sockfd, sender.msg, strlen(sender.msg), 0);
			send(sockfd, sender.msg, strlen(sender.msg), 0);
	}else { //writing state
		length = recv(sockfd, reciever.msg, BUFSIZE, 0); //saves the length of the message
		reciever.msg[length] = '\0'; //trims it nicely
		switch(color_choice){
			case 1: printf(ANSI_COLOR_YELLOW "%s:%s",user, reciever.msg);break;
			case 2: printf(ANSI_COLOR_MAGENTA "%s:%s",user, reciever.msg);break;
			case 3: printf(ANSI_COLOR_CYAN "%s:%s",user, reciever.msg);break;
			default: printf("aiurea\n");exit(0);
		}
		fflush(stdout);
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr){
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

void login(struct UserStruct basic[4],struct UserStruct connected[4],char user[20], char pass[20]){
	int flag = 0;
	
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			if( (strcmp(basic[i].user, user) == 0) && (strcmp(basic[i].pass, pass) == 0) && 
				(strcmp(user, connected[j].user) != 0) && (strcmp(pass, connected[j].pass) != 0)){
					printf("Permission granted!\n");
					flag = 1;
					strcpy(connected[j].user,user);
					strcpy(connected[j].pass,pass);
			}
		}
	}	
	if(flag == 0){
		printf("Invalid user or password!\n");
		exit(0);
	}

}
	
int main(){
	int sockfd, fdmax, i, color_choice;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	struct UserStruct basic[4];
	struct UserStruct connected[4];

	char user[20], pass[20];
	printf("Enter your username:");scanf("%s",user);
	printf("Enter your password:");scanf("%s",pass);

	//programming at its finest!
	strcpy(basic[0].user,"bogdan");		strcpy(basic[0].pass,"pocol");	//basic[0].in_use = 0;
	strcpy(basic[1].user,"alexandru");	strcpy(basic[1].pass,"rat");	//basic[1].in_use = 0;
	strcpy(basic[2].user,"elena");		strcpy(basic[2].pass,"ghitan");	//basic[2].in_use = 0;
	strcpy(basic[3].user,"georgiana");	strcpy(basic[3].pass,"filip");	//basic[3].in_use = 0;

	login(basic,connected,user,pass);

	printf("\nPick the text color:\n");
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
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("Nothing to SELECT bro");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_message(i, sockfd, color_choice,user);
	}
	printf("Client-quited\n");
	close(sockfd);
	return 0;
}