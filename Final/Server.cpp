#include "Server.h"

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

void Server::Start() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);
	if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}
	return;
}

void Server::ListenForConnections() {
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}
	SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
	}

	p1.socket = clientSocket;

	clientSocket = accept(serverSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
	}
	p2.socket = clientSocket;
	

	std::thread t1(&Server::Listen, this, &p1, 1);
	std::thread t2(&Server::Listen, this, &p2, 2);
	t1.join();
	t2.join();
}

void Server::Listen(Player* p, int playerNum) {
	while (true) {
		string command = Recieve(p->socket);
		stringstream ssCommand;
		ssCommand << command;
		string parsedCommand;
		ssCommand >> parsedCommand;
		
		if (parsedCommand == "START") {
			if (!isStarted) StartGame(playerNum);
		}
		else if (parsedCommand == "MOVE") {
			if (turn == p->sign) {
				Send("INVALID_MOVE\n", p->socket);
				continue;
			}
			int row = 0;
			int column = 0;
			ssCommand >> row;
			ssCommand >> column;
			if (board[row][column] != '_') {
				Send("INVALID_MOVE\n", p->socket);
				continue;
			}
			board[row][column] = p->sign;
			turn = p->sign;
			SendBoard();
		}
		else if (parsedCommand == "QUIT") {
			break;
		}
	}

}

void Server::StartGame(int started) {
	isStarted = true;
	mP1.lock();
	mP2.lock();
	if (started == 1) {
		p1.sign = 'X';
		p2.sign = 'O';
	}
	else if (started == 2) {
		p2.sign = 'X';
		p1.sign = 'O';
	}
	string message1 = "START ";
	message1 += p1.sign;
	message1 += '\n';
	string message2 = "START ";
	message2 += p2.sign;
	message2 += '\n';
	Send(message1, p1.socket);
	Send(message2, p2.socket);
	mP1.unlock();
	mP2.unlock();
}

void Server::SendBoard() {
	string sBoard = "BOARD ";
	for (int i = 0; i < 3; i++) {
		sBoard += '\n';
		for (int j = 0; j < 3; j++) {
			sBoard += board[i][j];
		}
	}
	sBoard += '\n';
	mP1.lock();
	mP2.lock();
	Send(sBoard, p1.socket);
	Send(sBoard, p2.socket);
	mP1.unlock();
	mP2.unlock();
}

void Server::CheckBoard(char sign) {
	//check column
	for (int i = 0; i < 3; i++) {
		if (board[0][i] == sign) {
			if (board[1][i] == sign && board[2][i] == sign) {
				Win(sign);
				return;
			}
		}
	}

	//check row
	for (int i = 0; i < 3; i++) {
		if (board[i][0] == sign) {
			if (board[i][1] == sign && board[i][2] == sign) {
				Win(sign);
				return;
			}
		}
	}

	//check diag
	if (board[0][0] == sign) {
		if (board[1][1] == sign && board[2][2] == sign) {
			Win(sign);
			return;
		}
	}

	if (board[0][2] == sign) {
		if (board[1][1] == sign && board[2][0] == sign) {
			Win(sign);
			return;
		}
	}

	//check draw
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (board[i][j] == '_') {
				Draw();
				return;
			}
		}
	}
}

void Server::Win(char sign) {
	string message = "GAME_OVER";
	message += sign;
	message += " WIN\n";
	mP1.lock();
	mP2.lock();
	Send(message, p1.socket);
	Send(message, p2.socket);
	mP1.unlock();
	mP2.unlock();
	Close();
}

void Server::Draw() {
	string message = "GAME_OVER DRAW\n";
	mP1.lock();
	mP2.lock();
	Send(message, p1.socket);
	Send(message, p2.socket);
	mP1.unlock();
	mP2.unlock();
	Close();
}

void Server::Disconect() {
	string message = "OPPONENT_DISCONNECTED\n";
	mP1.lock();
	mP2.lock();
	Send(message, p1.socket);
	Send(message, p2.socket);
	mP1.unlock();
	mP2.unlock();
}

void Server::Close() {
	closesocket(serverSocket);
	WSACleanup();
}
