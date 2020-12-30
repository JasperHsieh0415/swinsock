/*
	Simple Windows socket program
*/
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wchar.h>
#include <string>
#include <array>
#include <csignal>
#include <iostream>
#include <errno.h>
#include "getopt.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define BUFLEN 1024 * 100
enum {
	PROTOCOL_UDP = 1,
	PROTOCOL_TCP
};
enum {
	SOCK_SERVER,
	SOCK_CLIENT
};

int exit_flag = 0;
int sock_port = 8989;
int file_save = 0;
PCSTR sock_ip;
FILE* stream;

using namespace std;

void signalHandler(int signum)
{
	cout << "Interrupt signal (" << signum << ") received.\n";
	
	exit_flag = 1;
}

void show_help()
{
	printf("Simple Windows socket service (swinsock)\n");
	printf("Usage: swindock [OPTION]\n\n");
	printf("Main options:\n");
	printf("-h		show help\n");
	printf("-v		show version\n");
	printf("-s		enable file save\n");
	printf("-t [s/c]	set socket type to TCP server(s)/client(c)\n");
	printf("-u [s/c]	set socket type to UDP server(s)/client(c)\n");
	printf("-i addr		specify remote server address\n");
	printf("-p port		specify socket port number\n");
}

void show_version()
{
	printf("Simple Windows socket service (swinsock)\n");
	printf("build date: %s %s\n", __DATE__, __TIME__);
}

int tcp_server_init(int* sockfd)
{
	int ret = 0;
	int fd = 0;
	struct sockaddr_in si_client;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	int client_len = sizeof(si_client);
	char rcv_buf[BUFLEN];
	int recv_bytes = 0;

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(sock_port);
	si_other.sin_addr.s_addr = INADDR_ANY;

	/* Bind */
	if (bind(*sockfd, (struct sockaddr*)&si_other, sizeof(si_other)) < 0)
	{
		printf("Bind fail!\n");
		return -1;
	}
	listen(*sockfd, 1); //TCP service one client one time

	//send start message
	while (1) {

		if (exit_flag)
			break;

		printf("Ready to accept client\n");
		fd = accept(*sockfd, (struct sockaddr*)&si_client, (socklen_t*)&client_len);
		if (fd < 0) {
			printf("accept failed\n");
			continue;
		}
		else {
			int one = 1;
			if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one)) != 0)
				perror("setsockopt()");
		}

		printf("TCP Server Ready to reveive data\n");
		recv_bytes = recv(fd, rcv_buf, BUFLEN, 0);
		if (recv_bytes == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			break;
		}
		else
		{
			if (file_save && stream)
			{
				fwrite(rcv_buf, recv_bytes, 1, stream);
			}
			printf("Receive %d bytes\n", recv_bytes);
		}
	}
	closesocket(fd);
	return ret;
}

int tcp_client_init(int* sockfd)
{
	int ret = 0;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buf[BUFLEN];
	int recv_bytes = 0;

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(sock_port);

	if (!sock_ip || !sock_ip[0]) {
		printf("Remote IP address is null, -i to add server address\n");
		return 0;
	}

	inet_pton(AF_INET, sock_ip, &si_other.sin_addr.S_un.S_addr);

	if (connect(*sockfd, (struct sockaddr*)&si_other, sizeof(si_other)) < 0) {
		printf("connect error\n");
			return 0;
	}
	printf("TCP client Ready to reveive data\n");
	//send start message
	while (1) {

		if (exit_flag)
			break;
		recv_bytes = recv(*sockfd, buf, BUFLEN, 0);
		if (recv_bytes == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			break;
		}
		else
		{
			if (file_save && stream)
			{
				fwrite(buf, recv_bytes, 1, stream);
				printf("Receive %d bytes\n", recv_bytes);
			}
			
		}
	}
	return ret;
}


int udp_server_init(int* sockfd)
{
	int ret = 0;
	struct sockaddr_in si_client;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	int client_len = sizeof(si_client);
	char buf[BUFLEN];
	int recv_bytes = 0;

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(sock_port);
	si_other.sin_addr.s_addr = INADDR_ANY;
	
	/* Bind */
	if (bind(*sockfd, (struct sockaddr*)&si_other, sizeof(si_other)) < 0)
	{
		printf("Bind fail!\n");
		return -1;
	}

	//send start message
	while (1) {

		if (exit_flag)
			break;

		memset(buf, '\0', BUFLEN);
		printf("UDP Server Ready to reveive data\n");
		recv_bytes = recvfrom(*sockfd, buf, BUFLEN, 0, (struct sockaddr*)&si_client, &client_len);
		if (recv_bytes == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
		}
		else
		{
			if (file_save && stream)
			{
				fwrite(buf, recv_bytes, 1, stream);
			}


			printf("Receive %d bytes\n", recv_bytes);
		}
	}
	return ret;
}


int udp_client_init(int *sockfd)
{
	int ret = 0;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	int recv_bytes = 0;

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(sock_port);

	if (!sock_ip || !sock_ip[0]) {
		printf("Remote IP address is null, -i to add server address\n");
		return 0;
	}

	inet_pton(AF_INET, sock_ip, &si_other.sin_addr.S_un.S_addr);
	
	//send start message
	sprintf_s(message, "%s", "start");
	if (sendto(*sockfd, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == -1)
	{
		perror("sendto()");
		return -1;
	}

	while (1) {

		if (exit_flag)
			break;

		memset(buf, '\0', BUFLEN);
		printf("Ready to reveive data\n");
		recv_bytes = recvfrom(*sockfd, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);
		if (recv_bytes == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
		}
		else
		{
			if (file_save && stream)
			{
				fwrite(buf, recv_bytes, 1, stream);
			}
			printf("Receive %d bytes\n", recv_bytes);
		}
	}
	return ret;
}

int open_file_name(int net_type, int ser_type)
{
	char namebuf[128];
	SYSTEMTIME lt = { 0 };
	GetLocalTime(&lt);
	if (net_type == SOCK_STREAM) {
		if (ser_type == SOCK_SERVER)
			snprintf(namebuf, 127, "tcp_server_%02d_%02d_%02d", lt.wHour, lt.wMinute, lt.wSecond);
		else if (ser_type == SOCK_CLIENT)
			snprintf(namebuf, 127, "tcp_client_%02d_%02d_%02d", lt.wHour, lt.wMinute, lt.wSecond);
	}
	else {
		if (ser_type == SOCK_SERVER)
			snprintf(namebuf, 127, "udp_server_%02d_%02d_%02d", lt.wHour, lt.wMinute, lt.wSecond);
		else if (ser_type == SOCK_CLIENT)
			snprintf(namebuf, 127, "udp_client_%02d_%02d_%02d", lt.wHour, lt.wMinute, lt.wSecond);
	}
	namebuf[strlen(namebuf)] = 0;
	printf("filename = %s\n", namebuf);
	stream = fopen(namebuf, "w+b");
	return 0;
}


int main(int argc, char* argv[])
{
	int sockfd = 0;
	int net_type = 0;
	int ser_type = SOCK_SERVER;
	int result = 0;
	WSADATA wsa;

	signal(SIGINT, signalHandler);

	if (argc > 1)
	{
		while (-1 != (result = getopt(argc, argv, "p:i:t:u:svh")))
		{
			switch (result)
			{
			case 'h':
				show_help();
				return 0;
			case 'v':
				show_version();
				return 0;
			case 't':
				net_type = SOCK_STREAM;
				if (!memcmp(optarg, "s", 1)) {
					printf("Select TCP Server\n");
					ser_type = SOCK_SERVER;
				}
				else {
					printf("Select TCP Client\n");
					ser_type = SOCK_CLIENT;
				}
				break;
			case 'u':
				net_type = SOCK_DGRAM;
				if (!memcmp(optarg, "s", 1)) {
					printf("Select UDP Server\n");
					ser_type = SOCK_SERVER;
				}
				else {
					printf("Select UDP Client\n");
					ser_type = SOCK_CLIENT;
				}
				break;
			case 'p':
				sock_port = atoi(optarg);
				break;
			case 'i':
				sock_ip = optarg;
				break;
			case 's':
				file_save = 1;
				break;
			}
		}
	}
	else
	{
		show_help();
	}
	
	if (net_type) {
		printf("Initializing Winsock...\n");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//create socket
		if ((sockfd = socket(AF_INET, net_type, 0)) == SOCKET_ERROR)
		{
			printf("socket() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	if (file_save) {
		open_file_name(net_type, ser_type);
	}

	if (net_type == SOCK_DGRAM) {
		if (ser_type == SOCK_SERVER)
			udp_server_init(&sockfd);
		else
			udp_client_init(&sockfd);
	}
	else if (net_type == SOCK_STREAM) {
		if (ser_type == SOCK_SERVER)
			tcp_server_init(&sockfd);
		else
			tcp_client_init(&sockfd);
	}
	closesocket(sockfd);
	WSACleanup();
	fclose(stream);

	return 0;
}