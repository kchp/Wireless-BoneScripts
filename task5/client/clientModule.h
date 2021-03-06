
#ifndef CLIENTMODULE_H_
#define CLIENTMODULE_H_

#include <netinet/in.h>
#include <stdio.h>
#include <string>

using namespace std;

class client
{

public:
	client();
	~client();
	void clientSetup();
	void timerSetup();
	void clientSend(string RPC_str);
	void clientRecv();

private:
	int socket_fd;
	int port;
	const char* beagle;

	int bytes;
	struct sockaddr_in server_addr;
	char buffer[256];
	char msg[100];
};

#endif
