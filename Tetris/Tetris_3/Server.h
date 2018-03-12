#pragma once
#include <iostream>
#include <WinSock2.h>
#include "Data.h"
#include "Consola.h"
using namespace std;

class Server {
	WSADATA wsaData;		
	SOCKET hServSock, hSocket[4];
	SOCKADDR_IN servAddr, clntAddr[4];

	bool _Thread;			// 쓰레드 생존여부
	bool _Thread2;			// 쓰레드 생존여부
	bool player_exit[4];	// 플레이어 접속 여부
	bool player_lose[4];	// 패배 여부
	bool game_start;		// 게임시작
	int index;
	char server_ip[20];

	HANDLE hThread, hThread2;
	DWORD ThreadID, ThreadID2;
	char message[10000];
public:
	Server();
	void error(char str[]);
	void putsXY(int x, int y, char str[]);
	void start();
	void shutdown();
	int howManyPlayer();
	void letsStart();
	static DWORD WINAPI Server::thread1( void* arg );
	static DWORD WINAPI Server::thread2( void* arg );
	DWORD Server::Thread1_Main();
	DWORD Server::Thread2_Main();
};