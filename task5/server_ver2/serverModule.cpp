#include "json_protocol_server.h"
#include "serverModule.h"
#include "LM35DZ.h"
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

using namespace std;

FILE *fp;
double temperature;

server::server() {
	bytes = 0;
	getTemp = 0;
	exitTemp = 0;
	setTemp = 0;
	tempValue = 0;

	socket_fd = 0;
	new_socket_fd = 0;
	client_size = 0;
	port = 1955;
	fp = NULL;
}

server::~server() {
	fclose(fp);
	close(new_socket_fd);
	close(socket_fd);
	offLED();
}

void server::serverSetup() {

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		fprintf(stderr, "socket()\n");

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			== -1)
		fprintf(stderr, "bind()\n");
}

void server::daemonize() {
	pid_t process_id = 0;
	pid_t session_id = 0;
	process_id = fork();

	if (process_id < 0) {
		fprintf(stderr, "Error from fork.\n");
		exit(1);
	}

	if (process_id > 0) {
		fprintf(stdout, "Child PID <%d>\n", process_id);
		exit(0);
	}

	umask(0);
	session_id = setsid();
	if (session_id < 0) {
		fprintf(stderr, "setsid failed.\n");
		exit(1);
	}

	chdir("/");
	close(0);
	close(1);
	close(2);

	fp = fopen("/var/log/messages", "w+");
	fprintf(fp, "Syslog is running...\n");
}

void terminateHandler(int signum) {
	if (signum == 1) {
		exit(0);	//	If signal is recevied, terminate program
	}
}

void timerHandler(int signum) {
	if (signum == 14) {
		temperature = readTemp();
		fprintf(fp, "Temperature is %.2f °C\n", temperature);
		fflush(fp);
	}
}

void server::signalSetup() {
	if (signal(1, terminateHandler) == SIG_ERR)	//	install the SIGHUB signal_handler to terminate program
		fprintf(stderr, "Could not install handler - SIGHUB: [%d]...\n", 1);

	if (signal(14, timerHandler) == SIG_ERR)//	install the SIGVTALRM signal handler to read temp value after time configs
		fprintf(stderr, "Could not install handler - SIGVTALRM [%d]...\n", 14);
}

void server::timerSetup() {
	struct itimerval timer;

	//	Configure timer - 15sec:
	timer.it_value.tv_sec = 15;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 15;
	timer.it_interval.tv_usec = 0;
	int retval = setitimer(ITIMER_REAL, &timer, NULL);//	Timer is configured upon real time...
	if (retval < 0) {
		fprintf(fp, "Couldn't install the timer...\n");
		exit(1);
	}
}

void server::clientHandler() {
	const char *error = "Something wrong was unpacked...\n";
	const char *RPC_server;
	char tempValue[50];
	listen(socket_fd, 5);
	fprintf(fp, "Server pending...");

	client_size = sizeof(client_addr);
	bzero(buffer, 256);

	if ((new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr,
			&client_size)) == -1)
		fprintf(stderr, "accept\n");

	for (;;) {
		sleep(1);
		fflush(fp);

		sprintf(msg, "Enter RPC method(GETTEMP)>>");
		bytes = send(new_socket_fd, msg, strlen(msg), 0);
		if (bytes < 0)
			fprintf(fp, "Error occured while writing to socket\n");
		bzero(msg, 256);	//	Clear buffer, otherwise you will have double content...

		//	Receive data from client:
		bytes = recv(new_socket_fd, buffer, 255, 0);
		if (bytes < 0)
			fprintf(fp, "Error occured while reading from socket\n");

		//	Verify RPC call
		if (RPCDecode_server(buffer) == 0) {
			sprintf(tempValue, "temperature %.2lf°C", readTemp());	//	Enter temperature value
			RPC_server = RPCEncode_server(tempValue);				//	Combine temperature value in JSON string
			strcpy(msg, RPC_server);								//	Insert RPC call into msg buffer

			bytes = send(new_socket_fd, msg, strlen(msg), 0);
			if (bytes < 0)
				fprintf(fp, "Error occured while writing to socket\n");
		}
		else {
			bytes = send(new_socket_fd, error, strlen(error), 0);
			if (bytes < 0)
				fprintf(fp, "Server: Error during decoding of client-RPC answer...\n");
		}

	}
}
