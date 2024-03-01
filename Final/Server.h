#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <time.h>
#include <thread>
#include <iostream>
#include <winsock2.h>
#include <mswsock.h>
#include <filesystem>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

using namespace std;



class Server
{
	struct Player {
		SOCKET socket;
		char sign;
	};

	WSADATA wsaData;

	const int port = 12345;
	SOCKET serverSocket;
	sockaddr_in serverAddr;
	mutex mP1;
	Player p1;
	mutex mP2;
	Player p2;
	char turn = 'O';
	bool isStarted = false;
	bool ended = false;

	char board[3][3] = { {'_', '_', '_'},
						 {'_', '_', '_'},
						 {'_', '_', '_'} };

	void StartGame(int started);
	void SendBoard();
	void CheckBoard(char sign);
	void Listen(Player* p, int playerNum);
	void Win(char sign);
	void Draw();
	void Disconect();
public:
	void Start();
	void ListenForConnections();
	void Close();
};