#include "Client.h"

void Send(string message, SOCKET reciever) {
	const char messageSize = message.size();
	send(reciever, &messageSize, 1, 0);
	send(reciever, message.c_str(), message.size(), 0);
}

string Recieve(SOCKET sender) {
	char messageSize = 0;
	recv(sender, &messageSize, 1, 0);
	if ((int)messageSize <= 0) return "";
	vector<char> message((int)messageSize);
	recv(sender, &message[0], (int)messageSize, 0);
	string str = "";
	str.append(message.cbegin(), message.cend());
	return str;
}


void Client::Start() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}
}

void Client::ConnectToServer() {
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	InetPton(AF_INET, serverIp, &serverAddr.sin_addr);
	if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	cout << "Connection succeded" << endl;

	thread sendThread(&Client::Sender, this);
	thread recieveThread(&Client::Reciever, this);
	sendThread.join();
	recieveThread.join();
}

void Client::Sender() {
	string command;
	while (true) {
		getline(cin, command);
		if (command == "QUIT") {
			Send(command, clientSocket);
			Close();
		}
		Send(command, clientSocket);
	}
}

void Client::Reciever() {
	string command;
	while (true) {
		command = Recieve(clientSocket);
		cout << command;
	}
}

void Client::Close() {
	closesocket(clientSocket);
	WSACleanup();
}
