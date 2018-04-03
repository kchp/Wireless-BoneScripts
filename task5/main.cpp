
#include "LM35DZ.h"
#include "serverModule.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>

using namespace rapidjson;
using namespace std;

int main(int argC, char **argV)
{
	server Server;

	Server.serverSetup();
	Server.daemonize();
	Server.signalSetup();
	Server.timerSetup();
	Server.clientHandler();

	return 0;
}
