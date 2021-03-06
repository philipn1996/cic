#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>                                                                                                         
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <strings.h>

//Socket definitions
#define PORT 1024
#define BUFFER 256
#define CLIENTPIPE "/tmp/ic-client"

int main(int argc, char *argv[] ) {
	if(argc!=2) {
		//printf("Bitte einen String & eine  angeben.\n");
		exit(EXIT_FAILURE);
	}
	char mes[BUFFER];
	int i=0;
	int ret,fd;
/*	while(mes[i]=argv[1][i]) {
		i++;
		if(i==BUFFER-1) {
			mes[i]='\0';
			break;
		}
	}
	for(;i<BUFFER-1;i++) {
		mes[i]='\0';
	}
*/	

	struct sockaddr_in serv_addr;
	char ip[sizeof(struct in_addr)];
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
		
	if( inet_pton(AF_INET, (const char *) argv[1], &ip) == -1) {
		perror("inet_pton");
		exit(EXIT_FAILURE);
	}
	serv_addr.sin_family = AF_INET;
	bcopy((char *)ip, (char *)&serv_addr.sin_addr.s_addr,sizeof(ip));
	serv_addr.sin_port = htons(PORT);
	
	/* Pipe to UI */
	fd = open(CLIENTPIPE, O_RDONLY);

	/* Create Socket */
	int s_id = socket(AF_INET, SOCK_STREAM, 0);
	if(s_id==-1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	/* Connect */ 
	ret=connect(s_id, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
       	if(ret == -1) {
		printf("Der Fehler trat hier auf!\n");
		perror("connect");
		exit(EXIT_FAILURE);
	}
	while(1) {
		bzero(&mes, BUFFER);
		read(fd, mes, BUFFER);
		
		if(write(s_id, mes, sizeof(mes) -1) == -1) {
			perror("write");
		}
		
	}
	close(s_id);
	exit(EXIT_SUCCESS);
}

