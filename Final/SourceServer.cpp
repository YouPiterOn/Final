#include "Server.h"

int main() {
	Server server;
	server.Start();
	server.ListenForConnections();
	server.Close();
	return 0;
}