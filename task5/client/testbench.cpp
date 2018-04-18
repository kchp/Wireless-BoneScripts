
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "clientModule.h"
#include "json_protocol_client.h"
#include <cstdio>
#include <iostream>
#include <string>

using namespace rapidjson;
using namespace std;

int main(int argC, char **argV)
{
	if(argC != 2){
		cerr << "JSON document requires 1 method..." << endl;
		exit(1);
	}

	//	Pack up the JSON document to be transported from the client:
	const char *RPC_call = RPCEncode_client(argV[1]);

	client Client;
	Client.clientSetup();			//Connect client and server:

	Client.clientSend(RPC_call);	//	Transmit the RPC call
	Client.clientRecv();			//	Receive the temperature

	return 0;
}
