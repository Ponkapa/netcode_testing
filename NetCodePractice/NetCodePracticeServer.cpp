/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define MAXIMUM_BUFFER_LENGTH 100

void startWSA();
void *get_in_addr(struct sockaddr *sa);

int main(int argc, char *argv[])
{
	// Start WinSock2, needed for Windows socket usage
	startWSA();

	// Initialize variables

	int socket_file_descriptor;
	struct addrinfo configuration, *server_info, *result_iterator;
	int error_code;
	int number_of_bytes;
	struct sockaddr_storage client_address;
	char buffer[MAXIMUM_BUFFER_LENGTH];
	socklen_t address_length;
	char s[INET6_ADDRSTRLEN];

	// Allocate memory for configuration
	memset(&configuration, 0, sizeof configuration);
	configuration.ai_family = AF_UNSPEC; // Sets IPv4 or IPv6, this is unspecified, so either type can connect to us
	configuration.ai_socktype = SOCK_DGRAM; // Sets the socket type we want to be Datagram, or UDP
	configuration.ai_flags = AI_PASSIVE; // Use my IP

	if ((error_code = getaddrinfo(NULL, DEFAULT_PORT, &configuration, &server_info)) != 0) {
		// Attempting to get the server information failed
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error_code));
	}

	for (result_iterator = server_info; result_iterator != NULL; result_iterator->ai_next) {
		if ((socket_file_descriptor = socket(result_iterator->ai_family, result_iterator->ai_socktype, result_iterator->ai_protocol)) == -1) {
			// Could not establish a socket with this reuslt
			perror("listener: socket");
			// No point in trying to do more with this one
			continue;
		}

		if (bind(socket_file_descriptor, result_iterator->ai_addr, result_iterator->ai_addrlen) == -1) {
			closesocket(socket_file_descriptor);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (result_iterator == NULL) {
		fprintf(stderr, "listener: failed to bind socket");
		return 2;
	}

	freeaddrinfo(server_info);

	address_length = sizeof client_address;

	if ((number_of_bytes = recvfrom(socket_file_descriptor, buffer, MAXIMUM_BUFFER_LENGTH - 1, 0, ((struct sockaddr *)&client_address), &address_length)) == -1) {
		exit(1);
	}

	printf("listener: got packet from %s\n", inet_ntop(client_address.ss_family, get_in_addr((struct sockaddr *)&client_address), s, sizeof s));
	printf("listener: packet is %d bytes long\n", number_of_bytes);
	buffer[number_of_bytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buffer);

	closesocket(socket_file_descriptor);

	return 0;
}

void startWSA() {
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}