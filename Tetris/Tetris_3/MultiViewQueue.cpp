#include "MultiViewQueue.h"



MultiViewQueue::MultiViewQueue()	{
	_Q = (Que*)malloc(sizeof(Que));
	_Q->front = 0;
	_Q->rear = 0;
}

MultiViewQueue::~MultiViewQueue()	{}

void MultiViewQueue::resetQueue()	{
	_Q->front = 0;
	_Q->rear = 0;
	free(_Q);
	_Q = (Que*)malloc(sizeof(Que));
}

void MultiViewQueue::inQueue(Data_1 *temp)	{
	if(isFull())	{
		return;
	}
	_Q->rear = (_Q->rear + 1) % MAX;
	_Q->data[_Q->rear] = temp;
}

Data_1* MultiViewQueue::deQueue()
{
	if(isEmpty())	{
		return NULL;
	}
	_Q->front = (_Q->front + 1) % MAX;
	return _Q->data[_Q->front];
}

bool MultiViewQueue::isEmpty()	{
	return _Q->front == _Q->rear;
}

bool MultiViewQueue::isFull()	{
	return _Q->front % MAX == (_Q->rear + 1) % MAX;
}