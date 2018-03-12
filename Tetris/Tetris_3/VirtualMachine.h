#pragma once
#include "Data.h"
#include "Tetrimino.h"

class VirtualMachine {
	int _virtual_map[23][14];
public:

	VirtualMachine();
	~VirtualMachine();
	void setBlockToMap(int x, int y, int block_type, int block_state, int what);	
	void copyMap(int dest[23][14], int source[23][14]);
	Data_2* getOptimum(Data_2 *temp);
	bool check(int x, int y, int block_type, int block_state);
	double getFx();
};







