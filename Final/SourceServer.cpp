#include "Server.h"

int main() {
	Server server;
	server.Start();
	server.ListenForConnections();
	return 0;
}