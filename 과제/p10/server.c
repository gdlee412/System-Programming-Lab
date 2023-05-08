#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAXLINE 80

int main (int argc, char* argv[]) {
	int listening_fd, connecting_fd, connecting_address_length;
	//struct hostent* host_entry;
	struct sockaddr_in socket_address, connecting_address;
	char buffer[MAXLINE], buffer_data[MAXLINE];

	if (argc < 2) {
		printf("Received %d arguments. Please enter port number!\n", argc - 1);
		exit(1);
	}

	uint32_t port = (uint32_t)strtol(argv[1], NULL, 10); // contains port number 

	/* Create listen socket */
	if((listening_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("socket() failed.\n");
		exit(2);
	}

	memset((char *)&socket_address, 0, sizeof(socket_address)); // initializes socket_address fields to zero
	socket_address.sin_family = AF_INET; // sets address family to IPv4 
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY); // copies ip address
	socket_address.sin_port = htons(port); // sets port after converting it to "network byte order"
	
	/* Bind sockaddr (IP, etc.) to listen socket */
	if(bind(listening_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
		printf("bind() failed.\n");
		exit(3);
	}

	/* Listen to listen socket */
	if(listen(listening_fd, 5) < 0) {
		printf("listen() failed.\n");
		exit(4);
	}

	/* Accept connection request from clients */
	connecting_address_length = sizeof(connecting_address);

	if((connecting_fd = accept(listening_fd, (struct sockaddr *)&connecting_address, (socklen_t *)&connecting_address_length)) < 0) {
		printf("accept() failed.\n");
		exit(5);
	}
	printf("Server: accept() success.\n");

	while (1) {

		ssize_t num_bytes;
		/* Receive filename & data */
		num_bytes = read(connecting_fd, buffer, MAXLINE);
		buffer[num_bytes - 1] = '\0';
		if(strncmp(buffer, "quit", num_bytes) == 0) {
			break;
		}
		
		strcat(buffer, "_copy");

		int fd;
		if((fd = open(buffer, O_CREAT | O_RDWR, 0755)) < 0) {
			printf("Error creating file.\n");
			exit(6);
		}

		num_bytes = read(connecting_fd, buffer_data, MAXLINE);
		printf("got %ld bytes from client.\n", num_bytes);

		write(connecting_fd, buffer_data, num_bytes);
		write(fd, buffer_data, num_bytes);


		
	}

	close(connecting_fd);

	return 0;
}


