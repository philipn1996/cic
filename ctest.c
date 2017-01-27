#include <curses.h>
#include <stdlib.h> //noetig fuer atexit()

#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <strings.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
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

void *readloop(void *arg) {
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

	if (pthread_create(&inThread, NULL, &readloop, NULL) != 0){
        perror("Konnte Thread nicht erzeugen");
        exit(EXIT_FAILURE);
    }

	if(fork()==0) { //child-prozess
		//char *args[] = {NULL, ""};
		//sprintf(args[1],"%p",fd[1]);
		if (execve("./server", NULL,NULL) != 0){
            perror("Ein Fehler beim starten des Servers ist aufgetreten!");
            exit(EXIT_FAILURE);
        }
		//write(fd[1], "Hallo", 6);
	}
	if(fork()==0) {
		char *client_args[] = {"client", "127.0.0.1"};
		if (execve("./client", client_args, NULL) != 0){
            perror("Ein Fehler beim starten des Clients ist aufgetreten!");
            exit(EXIT_FAILURE);
        }
	}

	fd = open(SERVERPIPE, O_RDONLY);
	while(1) {
		bzero(&buffer, sizeof(buffer));
  		read(fd, buffer, BUFFER);
		sem_wait(&screen);
		addmessage('l', buffer);
		sem_post(&screen);
	}
    // TODO: Unreachable code
	pthread_join(inThread, NULL);
	sem_destroy(&screen);
	return(0);

}

#pragma clang diagnostic pop