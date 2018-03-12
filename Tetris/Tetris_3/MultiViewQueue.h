#pragma once
#include <iostream>
#include "Data.h"
#define MAX 20	// �ִ� ������ ���� +1
using namespace std;

class MultiViewQueue {	// ����� ������ ���� ť
	typedef struct {
		Data_1 *data[MAX];
		int front, rear;
	}Que;
	Que *_Q;
public:
	MultiViewQueue();
	~MultiViewQueue();
	void resetQueue();
	void inQueue(Data_1 *temp);
	Data_1* deQueue();
	bool isEmpty();
	bool isFull();
};