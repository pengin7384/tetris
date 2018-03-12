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
	////////////////////////////////////////////////////////// 클래스
	MultiView View;
	VirtualMachine Virtual;
	Map Player_Map;
	Map Computer_Map;
	Server Serv;
	////////////////////////////////////////////////////////// 쓰레드
	HANDLE hThread, hThread2;
	DWORD ThreadID, ThreadID2;

	////////////////////////////////////////////////////////// 소켓
	WSADATA wsaData;					
	SOCKET hSocket;		
	SOCKADDR_IN servAddr, clntAddr;		
	//////////////////////////////////////////////////////////
	char _id[4][50];
	bool _Thread;	// 쓰레드 생존여부
	bool _Thread2;	// 노래쓰레드 생존여부
public:
	Control();
	~Control();
	void reset();	// 모든 클래스 멤버 변수 초기화 함수
	void resetControl(); // Control 멤버 변수 초기화
	void lose(bool multi=0); // 패배처리
	void win(); // 승리처리
	void __exit(); // 종료 메시지
	void MouseClick(int *x, int *y); // 마우스 클릭 구현
	void menu(); // 메뉴
	void downTimer(int &down_timer); // 한 칸 내리는 타이머 함수 (점점 빨리지게)
	void single(bool assistant, bool computer=false, int level=EASY); // 싱글플레이(트레이닝모드, 컴퓨터, 난이도)
	void clear(bool computer, int &player_clear);	// 라인 정리 
	void keyEvent(int player, int &timer, bool assistant=false); // 키입력 처리
	void multi_server(); // 멀티 서버 호출
	void error(char str[]); // 에러 처리
	void multi_client(bool server=0); // 멀티 클라이언트 (서버 접속)
	void multi(int player, Data_4 *Player_Data); // 멀티 플레이 (데이터 전송 및 수신)
	int sendData(Data_1 *temp);	// Map 데이터들을 전송하는 함수 (길이 리턴)
	int sendAttck(int player); // 공격 메세지를 보낸다.
	int sendLose(); // 패배 메세지를 보낸다.
	static DWORD WINAPI thread( void* arg );	// 쓰레드를 멤버로 할당하기위한 함수
	static DWORD WINAPI thread2( void* arg );
	DWORD Thread2_Main();	// 쓰레드
	DWORD Thread_Main();
	bool clickLeftButton(int x, int y);
	bool clickRightButton(int x, int y);
	bool click1Button(int x, int y);
	bool click2Button(int x, int y);
	bool click3Button(int x, int y);
	bool click4Button(int x, int y);
};