#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <mswsock.h>
#include <thread>
#include <mutex>
#include <vector>
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Client
{
	WSADATA wsaData;

	const int port = 12345;
	const PCWSTR serverIp = L"127.0.0.1";
	SOCKET clientSocket;
	sockaddr_in serverAddr;

public:
	void Start();
	void ConnectToServer();
	void Sender();
	void Reciever();
	void Close();
};
