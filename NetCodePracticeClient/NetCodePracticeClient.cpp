// NetCodePracticeClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVERPORT "27015"
void startWSA();
int main(int argc, char *argv[]) {
	startWSA();

	int socket_file_descriptor;
	struct addrinfo configuration, *server_information, *result_iterator;
	int error_code;
	int number_of_bytes;

	if (argc != 3) {
		fprintf(stderr, "usage: talker hostname message\n");
		exit(1);
	}

	memset(&configuration, 0, sizeof configuration);

	configuration.ai_family = AF_UNSPEC;
	configuration.ai_socktype = SOCK_DGRAM;

	if ((error_code = getaddrinfo(argv[1], SERVERPORT, &configuration, &server_information)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error_code));
		return 1;
	}

	for (result_iterator = server_information; result_iterator != NULL; result_iterator->ai_next) {
		if ((socket_file_descriptor = socket(result_iterator->ai_family, result_iterator->ai_socktype, result_iterator->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (socket_file_descriptor == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	if ((number_of_bytes = sendto(socket_file_descriptor, argv[2], strlen(argv[2]), 0, result_iterator->ai_addr, result_iterator->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	freeaddrinfo(server_information);

	printf("talker: sent %d bytes to %s\n", number_of_bytes, argv[1]);
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