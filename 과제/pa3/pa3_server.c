#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct _query {
    int user;
    int action;
    int data;
} query;

typedef struct _users_info {
	int password;
	int login_status;
} users_info;

typedef struct _reserved_seats {
	int status;
	int count;
	int seats[256];
} reserved_seats;

users_info users[1024];
int seats[256];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void init() {
	for(int i = 0; i < 256; i++) {
		seats[i] = -1;
	}

	for(int i = 0; i < 1024; i++) {
		users[i].password = -1;
		users[i].login_status = -1;
	}
}

int login(int id, int pw, int connfd) {
	if(users[id].password == -1) {
		users[id].password = pw;
		users[id].login_status = connfd;
		return 1;
	}

	if(users[id].password == pw && users[id].login_status == -1) {
		users[id].login_status = connfd;
		return 1;
	}
	return -1;
}

int reserve(int id, int seat, int connfd) {
	if(seat < 0 || seat > 255)
		return -1;

	if(users[id].login_status == -1 || users[id].login_status != connfd)
		return -1;

	if(seats[seat] == -1) {
		seats[seat] = id;
		return seat;
	}
	return -1;
}

reserved_seats check_reservation(int id, int connfd) {
	reserved_seats seat_info;
	seat_info.count = 0;
	seat_info.status = 1;

	if(users[id].login_status == -1 || users[id].login_status != connfd) {
		seat_info.status = -1;
		return seat_info;
	}

	for(int i = 0; i < 256; i++) {
		if(seats[i] == id) {
			seat_info.seats[seat_info.count++] = i;
		}
	}

	if(seat_info.count == 0)
		seat_info.status = -1;

	return seat_info;
}

int cancel_reservation(int id, int seat, int connfd) {
	if(seat < 0 || seat > 255)
		return -1;

	if(users[id].login_status == -1 || users[id].login_status != connfd)
		return -1;

	if(seats[seat] == id) {
		seats[seat] = -1;
		return seat;
	}

	return -1;
}

int logout(int id, int connfd) {
	if(users[id].login_status == connfd) {
		users[id].login_status = -1;
		return 1;
	}

	return -1;
}

void *tickets(void *arg) {
	int connfd = *((int*)arg);
	pthread_detach(pthread_self());
	free(arg);

	query q;
	ssize_t numBytes;
	int curr_login_users[1024] = {0};
	int login_count = 0;
	int result;
	while((numBytes = read(connfd, &q, sizeof(query))) > 0) {
		printf("%d %d %d", q.user, q.action, q.data);
		if(q.user < 0 || q.user > 1023 || q.data == -1) {
			result = -2;
			write(connfd, &result, sizeof(int));
			continue;
		}

		pthread_mutex_lock(&m);
		switch(q.action) {
			case 0: result = 256; break;
			case 1: result = login(q.user, q.data, connfd); break;
			case 2: result = reserve(q.user, q.data, connfd); break; 
			case 3: break;
			case 4: result = cancel_reservation(q.user, q.data, connfd);break ;
			case 5: result = logout(q.user, connfd); break;
			default: result = -2; break;
		}
		pthread_mutex_unlock(&m);
		if(q.action == 3) {
			pthread_mutex_lock(&m);
			reserved_seats reservation = check_reservation(q.user, connfd);
			write(connfd, &reservation, sizeof(reserved_seats));
			pthread_mutex_unlock(&m);
			continue;
		}
		write(connfd, &result, sizeof(int));

		if(q.action == 1 && result == 1)
			curr_login_users[login_count++] = q.user;

		if(q.action == 5 && result == 1) {
			for(int i = 0; i < login_count; i++) {
				curr_login_users[i] = 0;
				pthread_mutex_lock(&m);
				users[q.user].login_status = -1;
				pthread_mutex_unlock(&m);

				if(curr_login_users[i] == q.user) {
					for(int j = i; j < login_count; i++) {
						curr_login_users[j] = curr_login_users[j + 1];
					}
					login_count--;
					break;
				}
			}
		}

		if(q.action == 0) {
			for(int i = 0; i < login_count; i++) {
				pthread_mutex_lock(&m);
				users[curr_login_users[i]].login_status = -1;
				pthread_mutex_unlock(&m);
			}
			break;
		}
	}

	close(connfd);
	return NULL;
}

int main(int argc, char* argv[])
{
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
	
    if(argc < 2) {
    	printf("Received %d arguments. Please enter port number!\n", argc - 1);
    	exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	    return 0;

    if(listen(serverSocket, 1024) < 0)
	    return 0;

    /*
     * Insert your PA3 server code
     *
     * You should generate thread when new client accept occurs
     * and process query of client until get termination query
     *
     */

    init();
    int *connfdp;    
    while(1) {
    	struct sockaddr_in clientAddr;
    	ssize_t clientAddrlen;
   	connfdp = (int *)malloc(sizeof(int));

	if((*connfdp = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrlen)) < 0) {
		printf("accept() failed\n");
		free(connfdp);
		continue;
	}


	pthread_t tid;

	pthread_create(&tid, NULL, tickets, connfdp);
    }

    return 0;
}
