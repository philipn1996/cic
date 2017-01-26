#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>                                                                                                         
#include <netinet/in.h>

#include <signal.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

//Socket definitions
#define BUFFER 256
#define PORT 1024
#define SERVERPIPE "/tmp/ic-srv"

/* Socket-Variables neccessary for SIGINT-Handler */

static int s_id;
static int connection;

/* Terminate Socket when the server_addr is terminated */
void handler(void) {
		//printf("Shutdown\n");
		close(connection);
		close(s_id);
}

int main(int argc, char *argv[]) {
	int fd;
	//fd=strtol(argv[1], NULL, 10);
//	struct sigaction sa;
//	sa.sa_handler = handler;
//	sigemptyset(&sa.sa_mask);
	
//	if(sigaction(SIGINT, &sa, NULL) == -1)
//		perror("sigaction");
	atexit(handler);

	/* Prepare Socket-Variables */

	struct sockaddr_in server_addr, client_addr;
	char buffer[BUFFER];
	int ret, clilen;
	/*Create Socket*/
	//printf("Create Socket...\n");
	s_id = socket(AF_INET, SOCK_STREAM, 0);
	if(s_id==-1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	//printf("Socket created.\n");
	
	server_addr.sin_family=AF_INET;	
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	ret=bind(s_id, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        if(ret == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* Accept connections */

	if(listen(s_id, 5) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	fd = open(SERVERPIPE, O_WRONLY);

	//Ready! process connections

	while(1) {
		clilen = sizeof(client_addr);
		connection = accept(s_id, (struct sockaddr*)&client_addr, &clilen);
		if(connection == -1) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		while(1) {
			bzero(buffer, BUFFER);
			if( (ret=read(connection, buffer, BUFFER)) == -1 ) {
				perror("read");
				exit(EXIT_FAILURE);
			}
			write(fd, buffer, sizeof(buffer));
		}
		close(connection);
	
	}
}
