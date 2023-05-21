#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 80

int main (int argc, char *argv[]) {
    ssize_t num_bytes;
    int connected_fd;
    struct hostent *host_entry;
    struct sockaddr_in socket_address;
    char buffer[MAXLINE];

    if (argc < 3) {
        printf("Received %d arguments. Please enter host address and port number!\n", argc - 1);
        exit(1);
    }

    char* host = argv[1];
    uint16_t port = (uint16_t)strtol(argv[2], NULL, 10);

    if ((connected_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("socket() failed.\n"); 
        exit(2);
    }

    if ((host_entry = gethostbyname(host)) == NULL) {
        printf("invalid hostname %s\n", host); 
        exit(3);
    }

    memset((char *)&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    memcpy((char *)&socket_address.sin_addr.s_addr, (char *)host_entry->h_addr, (size_t) host_entry->h_length);
    socket_address.sin_port = htons(port);

    if (connect(connected_fd,(struct sockaddr *)&socket_address,sizeof(socket_address)) < 0) {
        printf("connect() failed.\n");
        exit(4);
    }
    
    sprintf(buffer, "Insert your name : ");
    write(STDOUT_FILENO, buffer, strlen(buffer));
    char username[10];
    ssize_t user_size;
    user_size = read(STDIN_FILENO, username, 11);
    username[user_size - 1] = '\0';

    while ((num_bytes = read(STDIN_FILENO, buffer, MAXLINE)) > 0) {
        write(connected_fd, buffer, (size_t) num_bytes);
        //write(connected_fd, username, (size_t) user_size);
	num_bytes = read(connected_fd, buffer, MAXLINE);
        write(STDOUT_FILENO, buffer, (size_t) num_bytes);
    }
    close(connected_fd);

    return 0;
}
