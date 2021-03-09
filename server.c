/*
 * server be sent file
 */
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

#define SERVER_PORT 6666
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int main(int argc, char **argv)
{
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("Create Socket Failed!\n");
		exit(1);
	}

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
		printf("Server Bind Port: %d Failed!\n", SERVER_PORT);
		exit(1);
	}

	if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE)) {
		printf("Server Listen Failed!\n");
		exit(1);
	}

	while(1) {
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);

		int new_server_socket = accept(server_socket,
				(struct sockaddr*)&client_addr,
				&length);

		if (new_server_socket < 0) {
			printf("Server Accept Failed!\n");
			break;
		}

		char buffer[BUFFER_SIZE];
		bzero(buffer, sizeof(buffer));
		length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
		if (length < 0) {
			printf("Server Recieve Data Failed!\n");
			break;
		}

		char file_name[FILE_NAME_MAX_SIZE + 1];
		bzero(file_name, sizeof(file_name));
		strncpy(file_name,
				buffer,
				strlen(buffer) > FILE_NAME_MAX_SIZE ?
				FILE_NAME_MAX_SIZE : strlen(buffer));

		FILE *fp = fopen(file_name, "w");
		if (fp == NULL) {
			printf("File:\t%s Can Not Open To Write!\n", file_name);
			exit(1);
		}

		bzero(buffer, sizeof(buffer));
		length = 0;
		while (length = recv(new_server_socket,
					buffer,
					BUFFER_SIZE,
					0)) {
			if (length < 0) {
				printf("Recieve Data From Client Failed!\n");
				break;
			}

			int write_length = fwrite(buffer,
						sizeof(char),
						length,
						fp);

			if (write_length < length) {
				printf("File:\t%s Write Failed!\n", file_name);
				break;
			}
			bzero(buffer, BUFFER_SIZE);
		}

		close(new_server_socket);
		printf("receive File: %s from client Finished!\n", file_name);
		fclose(fp);
	}
	close(server_socket);

	return 0;
}
