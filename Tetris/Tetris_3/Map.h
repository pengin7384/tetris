#pragma once
#include <iostream>
#include <time.h>
#include "Tetrimino.h"
#include "Data.h"

using namespace std;

class Map {
	int _x, _y;
	int _map[23][14];
	int _score;
	int _prev_block_type;
	int _block_type;
	int _block_state;
	int _hold_block;
	int _clear_line;
	bool _block_exit;
public:
	Map();
	~Map();
	void resetMap();		// �� �ʱ�ȭ
	void setBlockToMap(int x, int y, int block_type, int block_state, int what); // �ش� ��ǥ 4x4�� ��� �� ���� ����
	void lineAttack(int count=1);	// ���� ���� ����
	void eraseGoodPos();	// ���� ��� ����
	void showGoodPos(Data_2 *temp);	// ���� ���
	void updateExPath();	// ���� ���
	void eraseExPath(int x, int block_type, int block_state); // ���� ��� �����
	void blockDown();		// �� ��ĭ ������
	void die();				// ��
	void recvLineAttack();	// ���ΰ����� �������
	void moveBlock(int move_x, int move_y); // �� �̵�
	void loopDownBlock();	// �����̽���
	void rotationBlock();	// ȸ��
	bool holdBlock();		// Z Ȧ��
	bool createNewBlock();	// ���� ����
	bool check(int x, int y, int block_type, int block_state);		// �� �浹 ����
	bool checkWall(int x, int y, int block_type, int block_state);	// �� �� �浹 ����
	bool isBlockExit();		// ���� ����?
	int lineClear(int line=0); // ���� ����
	int movePos(Data_2* temp, bool _spacebar); // ��ǻ�Ͱ� ���ϴ� ��ġ �Է� ó��
	Data_1* getData(int player);
	Data_2* getData();
};