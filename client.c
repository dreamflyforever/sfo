/*
 * client send file name and receive file from server.
 *
 */

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define HELLO_WORLD_SERVER_PORT 6666
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

#if 1
#define pf(format, ...) \
	{printf("[%s : %s : %d] ", \
	__FILE__, __func__, __LINE__); \
	printf(format, ##__VA_ARGS__);}
#else
#define pf(format, ...) 
#endif

int main(int argc, char **argv)
{
	struct sockaddr_in client_addr;
	int client_socket;
	struct sockaddr_in server_addr;
	if (argc != 2) {
		printf("Usage: %s ServerIPAddress\n", argv[0]);
		exit(1);
	}

	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("Create Socket Failed!\n");
		exit(1);
	}

	if (bind(client_socket,
		(struct sockaddr*)&client_addr,
		sizeof(client_addr))) {
			printf("Client Bind Port Failed!\n");
			exit(1);
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;

	if (inet_aton(argv[1], &server_addr.sin_addr) == 0) {
		printf("Server IP Address Error!\n");
		exit(1);
	}

	server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
	socklen_t server_addr_length = sizeof(server_addr);

	if (connect(client_socket,
				(struct sockaddr*)&server_addr,
				server_addr_length) < 0) {
		printf("Can Not Connect To %s!\n", argv[1]);
		exit(1);
	}

	char file_name[FILE_NAME_MAX_SIZE + 1];
	bzero(file_name, sizeof(file_name));
	printf("Please Input File Name On Server:");
	scanf("%s", file_name);

	char buffer[BUFFER_SIZE];
	bzero(buffer, sizeof(buffer));
	strncpy(buffer,
		file_name,
		strlen(file_name) > BUFFER_SIZE ?
		BUFFER_SIZE : strlen(file_name));
	printf("buffer: %s\n", buffer);

	send(client_socket, buffer, BUFFER_SIZE, 0);

	FILE *fp = fopen(file_name, "r");
	if (fp == NULL) {
		printf("File:\t%s Not Found!\n", file_name);
	} else {
		bzero(buffer, BUFFER_SIZE);
		int file_block_length = 0;
		while ((file_block_length = fread(buffer,
						sizeof(char),
						BUFFER_SIZE,
						fp)) > 0) {
			printf("file_block_length = %d\n", file_block_length);

			if (send(client_socket,
				 buffer,
				 file_block_length,
				 0) < 0) {
				 printf("Send File:\t%s Failed!\n", file_name);
				break;
			}

			bzero(buffer, sizeof(buffer));
		}
		fclose(fp);
		printf("File:\t%s Transfer Finished!\n", file_name);
	}

	printf("send File: %s to Server[%s] Finished!\n", file_name, argv[1]);

	close(client_socket);
	return 0;
}
