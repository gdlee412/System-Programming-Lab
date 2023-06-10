#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _query {
    int user;
    int action;
    int seat;
} query;


typedef struct _reserved_seats {
	int status;
	int count;
	int seats[256];
} reserved_seats;

int main(int argc, char *argv[]) {
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    int input_type, fd;
    int input_count = 0;

    if (argc == 3) {
        /* Insert your code for terminal input */
	input_type = 0;
    } else if (argc == 4) {
        /* Insert your code for file input */
    	input_type = 1;
	if((fd = open(argv[3], O_RDONLY)) < 0) {
		printf("open() failed\n");
		exit(1);
	}
	
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	int count = 0;
	char c;
	while(read(fd, &c, 1) && size != count) {
		count++;
		if(c == '\n') {
			input_count++;
		}
	}
	lseek(fd, 0, SEEK_SET);

	dup2(fd, STDIN_FILENO);

	
	close(fd);
    } else {
        printf("Follow the input rule below\n");
        printf("==================================================================\n");
        printf("argv[1]: server address, argv[2]: port number\n");
        printf("or\n");
        printf("argv[1]: server address, argv[2]: port number, argv[3]: input file\n");
        printf("==================================================================\n");
        exit(1);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Connection failed\n");
        exit(1);
    }

    /*
     * Insert your PA3 client code
     *
     * You should handle input query
     *
     * Follow the print format below
     *
     * 1. Log in
     * - On success
     *   printf("Login success\n");
     * - On fail
     *   printf("Login failed\n");
     *
     * 2. Reserve
     * - On success
     *   printf("Seat %d is reserved\n");
     * - On fail
     *   printf("Reservation failed\n");
     *
     * 3. Check reservation
     * - On success
     *   printf("Reservation: %s\n");
     *   or
     *   printf("Reservation: ");
     *   printf("%d, ");
     *   ...
     *   printf("%d\n");
     * - On fail
     *   printf("Reservation check failed\n");
     *
     * 4. Cancel reservation
     * - On success
     *   printf("Seat %d is canceled\n");
     * - On fail
     *   printf("Cancel failed\n");
     *
     * 5. Log out
     * - On success
     *   printf("Logout success\n");
     * - On fail
     *   printf("Logout failed\n");
     *
     */

    query q;
    q.user = -1;
    q.action = -1;
    q.seat = -1;
    int curr_input = 0;
    int result;
    reserved_seats reservation;

    while(1) {
	    char c;
	    char buf[15] = "";
	    int index = 0;
	    while(read(STDIN_FILENO, &c, 1)) {
		if(c == '\n') {
			buf[index] = '\0';
			q.seat = atoi(buf);
			break;
		}

		buf[index] = c;
		index++;

		if(c == ' ') {
			buf[index] = '\0';
			if(q.user == -1)
				q.user = atoi(buf);
			else if(q.action == -1)
				q.action = atoi(buf);
			else
				q.seat = atoi(buf);

			index = 0;
		}
		
	    }

	    curr_input++;
	    write(client_socket, &q, sizeof(query));

	    if(q.action != 3) {
		    read(client_socket, &result, sizeof(int));
	    }
	    else {
		    read(client_socket, &reservation, sizeof(reserved_seats));
		    result = reservation.status;
	    }

	    if(result == -2) {
	    	printf("Invalid query\n");
	    }
	    else if(q.action == 1) {
		if(result == 1)
			printf("Login success\n");
		else
			printf("Login failed\n");		
	    }
	    else if(q.action == 2) {
	    	if(result >= 0)
			printf("Seat %d is reserved\n", result);
		else
			printf("Reservation failed\n");
	    }
	    else if(q.action == 3) {
	    	if(result == 1) {
			printf("Reservation: ");
			for(int i = 0; i < reservation.count; i++) {
				if(i == reservation.count - 1)
					printf("%d\n", reservation.seats[i]);
				else
					printf("%d, ", reservation.seats[i]);
			}
		}
		else printf("Reservation check failed\n");
	    }
	    else if(q.action == 4) {
		if(result >= 0)
			printf("Seat %d is canceled\n", result);
		else
			printf("Cancel failed\n");
	    }
	    else if(q.action == 5) {
		if(result == 1)
			printf("Logout success\n");
		else
			printf("Logout failed\n");
	    }

	    if(result == 256) {
		break;
	    }

	    if(input_type == 1) {
	    	if(curr_input == input_count) {
			break;
		}
	    }
	    
	    q.user = -1;
	    q.action = -1;
	    q.seat = -1;
    }

    if(input_type == 1) {
	    q.user = 0;
	    q.action = 0;
	    q.seat = 0;
	    write(client_socket, &q, sizeof(query));
	    read(client_socket, &result, sizeof(int));
    }
	
    close(client_socket);

    return 0;
}
