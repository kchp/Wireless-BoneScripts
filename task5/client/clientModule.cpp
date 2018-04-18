
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <csignal>
#include "clientModule.h"
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "json_protocol_client.h"

using namespace std;

client::client()
{
	bytes = 0;
	socket_fd = 0;
	beagle = "192.168.7.2";
	port = 1955;
}

client::~client(){

}

void client::clientSetup()
{

	if( (socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		fprintf(stderr, "socket()\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(beagle);

	int connection_status = connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(connection_status == -1)
	{
		fprintf(stderr, "Error occured when trying to establish connection to the specified server\n");
		exit(1);
	}

	else
	{
		printf("You are connected!\nNetwork details:\n");
		printf("--------------------------------------\n");
		printf("IP: %s\n", beagle);
		printf("Port: %d\n", port);
		printf("--------------------------------------\n");
	}
}

void client::timerSetup()
{
	struct itimerval timer;

	//	Configure timer - 15sec:
	timer.it_value.tv_sec = 15;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 15;
	timer.it_interval.tv_usec = 0;
	int retval = setitimer(ITIMER_REAL, &timer, NULL);	//	Timer is configured upon real time...
	if(retval < 0)
	{
		cerr << "Could not install timer" << endl;
		exit(1);
	}
}

void client::clientSend(string RPC_str)
{
	const char *str_in = RPC_str.c_str();
	if( (bytes = send(socket_fd, &str_in, strlen(str_in), 0)) == -1)
		cerr << "send()" << endl;
}

void client::clientRecv()
{
	//memset(&buffer, 0, sizeof(buffer));

	if( (bytes = recv(socket_fd, &buffer, sizeof(buffer), 0)) == -1)
		cerr << "recv()" << endl;

	if(RPCDecode_client(buffer) < 0)
		printf("RPC call has wrong format...\n");

	buffer[bytes] = '\0';
	printf("The JSON string recevied from connected server:\n");
	printf(buffer);
}
