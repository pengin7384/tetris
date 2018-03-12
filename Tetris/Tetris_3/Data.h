#pragma once
#define DATA_MAP	1
#define DATA_ID		3

/* 통신에 사용할 데이터 종류들을 저장

Type
0	타입을 알아보기위한 
1	맵데이터
2	맵데이터(xy좌표, 맵, 블럭타입, 블럭회전상태) - AI용
3	아이디
4	전체 플레이어 정보 (아이디, 접속여부)
5	게임 시작
6	공격
7	승리
8	패배
9	멀티 종료
10	

*/

typedef struct {
	int _type;
}Data_0;

typedef struct {
	int _type;				// 데이터의 종류
	int _player;			// 플레이어 고유 번호
	int _prev_block_type;	// 전 블럭
	int _hold_block;		// 홀드 블럭
	int _map[23][14];
}Data_1;

typedef struct {
	int _x, _y;
	int _block_type;		// 전 블럭
	int _block_state;
	int _map[23][14];
}Data_2;


typedef struct {
	int _type;
	char _id[50];
}Data_3;

typedef struct {
	int _type;
	char _ip[20];
	char _id[4][50];
	bool _player_exit[4];
}Data_4;


typedef struct {
	int _type;
	int _player_number;
}Data_5;

typedef struct {
	int _type;
	int _player;
}Data_6;

typedef struct {
	int _type;
}Data_7;

typedef struct {
	int _type;
}Data_8;


typedef struct {
	int _type;
}Data_9;

