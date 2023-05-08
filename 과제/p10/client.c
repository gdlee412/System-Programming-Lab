#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <fcntl.h>
#define MAXLINE 80

int main (int argc, char *argv[]) {
	struct hostent* host_entry; // contains information related to the host like IP address
	struct sockaddr_in socket_address;
	char buffer[MAXLINE], buffer_data[MAXLINE];
	int connecting_fd;

	if (argc < 3){
		printf("Received %d arguments. Please enter host address and port number!\n", argc - 1);
		exit(1);
	}

	char *host = argv[1]; // contains hostname (e.g. "www.skku.edu", "localhost", etc.) 
	uint32_t port = (uint32_t)strtol(argv[2], NULL, 10); // contains port number 

	/* Create socket */
	if((connecting_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("socket() failed.\n");
		exit(2);
	}	
	/* Get host information */
	if((host_entry = gethostbyname(host)) == NULL) {
		printf("invalid hostname %s\n", host);
		exit(3);
	}

	memset((char *)&socket_address, 0, sizeof(socket_address)); //initializes socket_address fields to zero
	socket_address.sin_family = AF_INET; // sets address family to IPv4
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY); // copies ip address
	socket_address.sin_port = htons(port); // sets port after converting it to "network byte order"

	/* Connect to host */
	if(connect(connecting_fd,(struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
		printf("connect() failed.\n");
		exit(4);
	}
	printf("Client: connect() success.\n");

	while (1) {
		ssize_t num_bytes;
		/* Send filename & data */
		num_bytes = read(STDIN_FILENO, buffer, MAXLINE);
		buffer[num_bytes - 1] = '\0';
		if(strncmp(buffer, "quit", num_bytes) == 0) {
			break;
		}

		write(connecting_fd, buffer, num_bytes);	
		int fd;

		if((fd = open(buffer, O_RDWR, 0755)) < 0) {
			printf("cannot open file or file does not exist.\n");
			exit(5);
		}

		num_bytes = read(fd, buffer_data, MAXLINE);
		write(connecting_fd, buffer_data, num_bytes);

		num_bytes = read(connecting_fd, buffer_data, MAXLINE);
		printf("Send %ld bytes to server.\n", num_bytes);

	}

	close(connecting_fd);

	return 0;
}


