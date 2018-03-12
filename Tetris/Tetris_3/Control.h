#pragma once
#pragma comment(lib,"winmm") 
#include <iostream>
#include <conio.h>
#include <WinSock2.h>
#include "Data.h"
#include "Tetrimino.h"
#include "Keycode.h"
#include "Map.h"
#include "MultiView.h"
#include "VirtualMachine.h"
#include "Server.h"

#define EASY	1
#define NORMAL	2
#define HARD	3
#define EXTREME	4

using namespace std;

class Control {
	////////////////////////////////////////////////////////// Ŭ����
	MultiView View;
	VirtualMachine Virtual;
	Map Player_Map;
	Map Computer_Map;
	Server Serv;
	////////////////////////////////////////////////////////// ������
	HANDLE hThread, hThread2;
	DWORD ThreadID, ThreadID2;

	////////////////////////////////////////////////////////// ����
	WSADATA wsaData;					
	SOCKET hSocket;		
	SOCKADDR_IN servAddr, clntAddr;		
	//////////////////////////////////////////////////////////
	char _id[4][50];
	bool _Thread;	// ������ ��������
	bool _Thread2;	// �뷡������ ��������
public:
	Control();
	~Control();
	void reset();	// ��� Ŭ���� ��� ���� �ʱ�ȭ �Լ�
	void resetControl(); // Control ��� ���� �ʱ�ȭ
	void lose(bool multi=0); // �й�ó��
	void win(); // �¸�ó��
	void __exit(); // ���� �޽���
	void MouseClick(int *x, int *y); // ���콺 Ŭ�� ����
	void menu(); // �޴�
	void downTimer(int &down_timer); // �� ĭ ������ Ÿ�̸� �Լ� (���� ��������)
	void single(bool assistant, bool computer=false, int level=EASY); // �̱��÷���(Ʈ���̴׸��, ��ǻ��, ���̵�)
	void clear(bool computer, int &player_clear);	// ���� ���� 
	void keyEvent(int player, int &timer, bool assistant=false); // Ű�Է� ó��
	void multi_server(); // ��Ƽ ���� ȣ��
	void error(char str[]); // ���� ó��
	void multi_client(bool server=0); // ��Ƽ Ŭ���̾�Ʈ (���� ����)
	void multi(int player, Data_4 *Player_Data); // ��Ƽ �÷��� (������ ���� �� ����)
	int sendData(Data_1 *temp);	// Map �����͵��� �����ϴ� �Լ� (���� ����)
	int sendAttck(int player); // ���� �޼����� ������.
	int sendLose(); // �й� �޼����� ������.
	static DWORD WINAPI thread( void* arg );	// �����带 ����� �Ҵ��ϱ����� �Լ�
	static DWORD WINAPI thread2( void* arg );
	DWORD Thread2_Main();	// ������
	DWORD Thread_Main();
	bool clickLeftButton(int x, int y);
	bool clickRightButton(int x, int y);
	bool click1Button(int x, int y);
	bool click2Button(int x, int y);
	bool click3Button(int x, int y);
	bool click4Button(int x, int y);
};