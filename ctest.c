#include <curses.h>
#include <stdlib.h> //noetig fuer atexit()
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER 256
#define SERVERPIPE "/tmp/ic-srv"
#define CLIENTPIPE "/tmp/ic-client"
void quit()
{
  endwin();
  unlink(SERVERPIPE);
  unlink(CLIENTPIPE);
}
static int pos=0;
static sem_t screen;

void addmessage(char p, char * mes) {
	int x,y;
	getyx(stdscr, y, x);

	curs_set(0);
	pos++;
	int col;
	char fs[10];
	if(p=='l') {
	       	col=1;
		fs[0] = '%';
		fs[1] = 's';
		fs[2] = '\0';
	} else {
		col=COLS/2;
		sprintf(fs, "%%%ds", col);
	}	
	mvprintw(pos, col, fs, mes);
	curs_set(1);	
	move(y,x);
	refresh();
}

void *readloop() {
	int fdc = open(CLIENTPIPE, O_WRONLY);

	char buffer[BUFFER];
	bzero(buffer, BUFFER);

	while(1) {
		move(LINES-1, 2);
		getstr(buffer);
		if(write(fdc, buffer, BUFFER) == -1) {
			perror("write");
		}
		sem_wait(&screen);
		move(LINES-1,2);
		clrtoeol();
		addmessage('r', buffer);
		bzero(buffer, BUFFER);
		sem_post(&screen);
	}
}

int main(int argc, char *argv[]) {
	char buffer[BUFFER];
	int fd;

	pthread_t inThread;
	initscr();
	atexit(quit);

	mvprintw(LINES-1, 0, ">>");
	curs_set(1);
	/* Mutex to prevent multiple writes at a time */
	sem_init(&screen, 0, 1);
		
	
	/* create the FIFO (named pipe) */
	mkfifo(SERVERPIPE, 0666);
	mkfifo(CLIENTPIPE, 0666);	

	pthread_create(&inThread, NULL, readloop, NULL);

	if(fork()==0) { //child-prozess
		//char *args[] = {NULL, ""};
		//sprintf(args[1],"%p",fd[1]);
		execve("./server.exe", NULL,NULL);
		//write(fd[1], "Hallo", 6);
	}
	if(fork()==0) {
		char *args[] = { "client.exe", ""};
		args[1] = argv[1];
		execve("./client.exe", argv, NULL);
	}

	fd = open(SERVERPIPE, O_RDONLY);
	while(1) {
		bzero(&buffer, sizeof(buffer));
  		read(fd, buffer, BUFFER);
		sem_wait(&screen);
		addmessage('l', buffer);
		sem_post(&screen);
	}
	pthread_join(inThread, NULL);
	sem_destroy(&screen);
	return(0);

}
