#pragma once
#include <iostream>
#include <process.h>
#include <Windows.h>
#include "MultiViewQueue.h"
#include "Consola.h"
#include "Tetrimino.h"
#include "Data.h"

using namespace std;

class MultiView {
	MultiViewQueue Q;
	HANDLE hThread_MultiView;
	DWORD ThreadID;

	int _score;
	int _standard_x[4];
	int _standard_y[4];
	int _old_map[4][23][14];
	int _prev_block_type[4];
	int _old_prev_block_type[4];
	bool _Thread_MultiView;				
public:
	MultiView();
	~MultiView();
	void resetMultiView();
	void printEndl(int num);
	void putsXY(int x,int y,char str[], int color=WHITE);
	void intro();
	void serverState();
	void drawScore();			// ������
	void sendScore(int line);
	void basic(int num);		// ������
	void drawTwoBox();
	void drawFourBox();
	void menu_1();
	void menu_2();
	void menu_3();
	void menu_4();
	void requestMapView(Data_1 *temp);	// ���� ť�� ������ ��ť
	void printID(Data_4 *temp);	
	void processView();					// ���� ť���� �����͸� ��ť�Ͽ� ���
	void startThread_MuliView();
	void stopThread_MuliView();
	static DWORD WINAPI Thread_MultiView( void* arg );
	DWORD Thread_MultiView_Main();
};