#pragma once
#define DATA_MAP	1
#define DATA_ID		3

/* ��ſ� ����� ������ �������� ����

Type
0	Ÿ���� �˾ƺ������� 
1	�ʵ�����
2	�ʵ�����(xy��ǥ, ��, ��Ÿ��, ��ȸ������) - AI��
3	���̵�
4	��ü �÷��̾� ���� (���̵�, ���ӿ���)
5	���� ����
6	����
7	�¸�
8	�й�
9	��Ƽ ����
10	

*/

typedef struct {
	int _type;
}Data_0;

typedef struct {
	int _type;				// �������� ����
	int _player;			// �÷��̾� ���� ��ȣ
	int _prev_block_type;	// �� ��
	int _hold_block;		// Ȧ�� ��
	int _map[23][14];
}Data_1;

typedef struct {
	int _x, _y;
	int _block_type;		// �� ��
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

