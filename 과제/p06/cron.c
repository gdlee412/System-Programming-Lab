#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	unsigned int pid;
	time_t t;
	struct tm tm;
	int fd;
	char *argv[3];
	char buf[512];
	int fd0, fd1, fd2;

	fd = open("./crontab", O_RDWR);
	pid = fork();
	
	if(pid == -1) return -1;
	if(pid != 0)
		exit(0);
	if(setsid() < 0)
		exit(0);
	if(chdir("/") < 0)
		exit(0);

	umask(0);

	close(0);
	close(1);
	close(2);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = open("/dev/null", O_RDWR);
	fd2 = open("/dev/null", O_RDWR);

	t = time(NULL);
	localtime_r(&t, &tm);

	while (1)
	{
		buf[0] = '\0';	

		//insert your code
		
		char c;
		int i = 0;

		while(read(fd, &c, 1) != 0) {
			if(c == '\n' || c == '\0') {
				break;
			}
			buf[i] = c;
			i++;
		}
		buf[i] = '\0';

		//##  hints  ##
		char *token;
		char *pos = buf;
		i = 0;

		while((token = strtok_r(pos, " ", &pos))) {
			argv[i] = token;
			i++;
		}

		int min, hour;

		if(strncmp(argv[0], "*", 1) == 0) {
			min = -1;
		}
		else {
			min = atoi(argv[0]);
		}

		if(strncmp(argv[1], "*", 1) == 0) {
			hour = -1;
		}
		else {
			hour = atoi(argv[1]);
		}

		//int status;
		//unsigned int pid2;

		if((min == -1 || tm.tm_min == min) && (hour == -1 || tm.tm_hour == hour)) {
			if(fork() == 0) {
				execl("/bin/sh", "/bin/sh", "-c", argv[2], (char*) NULL);
			}
			wait(NULL);
			//waitpid(pid2, &status, WNOHANG);
		
		}
		else if(tm.tm_min != min) {
			if(tm.tm_min > min) {
				sleep((60 - tm.tm_min + min) * 60);
			}
			else {
				sleep((min - tm.tm_min) * 60);
			}
		}
		else if(tm.tm_hour != hour) {
			if(tm.tm_hour > hour) {
				sleep((24 - tm.tm_hour + hour) * 3600);
			}
			else {
				sleep((hour - tm.tm_hour) * 3600);
			}
		}
		
		t = time(NULL);
		localtime_r(&t, &tm);
	
		sleep(60 - tm.tm_sec % 60);
	}

	return 0;
}

