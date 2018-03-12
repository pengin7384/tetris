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
	void resetMap();		// 맵 초기화
	void setBlockToMap(int x, int y, int block_type, int block_state, int what); // 해당 좌표 4x4에 어떠한 블럭 값을 세팅
	void lineAttack(int count=1);	// 라인 공격 당함
	void eraseGoodPos();	// 최적 경로 삭제
	void showGoodPos(Data_2 *temp);	// 최적 경로
	void updateExPath();	// 예상 경로
	void eraseExPath(int x, int block_type, int block_state); // 예상 경로 지우기
	void blockDown();		// 블럭 한칸 내리기
	void die();				// 끝
	void recvLineAttack();	// 라인공격을 받을경우
	void moveBlock(int move_x, int move_y); // 블럭 이동
	void loopDownBlock();	// 스페이스바
	void rotationBlock();	// 회전
	bool holdBlock();		// Z 홀드
	bool createNewBlock();	// 새블럭 생성
	bool check(int x, int y, int block_type, int block_state);		// 블럭 충돌 여부
	bool checkWall(int x, int y, int block_type, int block_state);	// 블럭 벽 충돌 여부
	bool isBlockExit();		// 블럭이 존재?
	int lineClear(int line=0); // 라인 정리
	int movePos(Data_2* temp, bool _spacebar); // 컴퓨터가 원하는 위치 입력 처리
	Data_1* getData(int player);
	Data_2* getData();
};