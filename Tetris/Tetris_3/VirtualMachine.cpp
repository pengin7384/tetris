#include "VirtualMachine.h"

VirtualMachine::VirtualMachine()	{}
VirtualMachine::~VirtualMachine()	{}

void VirtualMachine::setBlockToMap(int x, int y, int block_type, int block_state, int what)	// 맵 (x,y)에 ?? 블럭 설정
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j])
				_virtual_map[y+i+1][x+j+2] = what;
}

void VirtualMachine::copyMap(int dest[23][14], int source[23][14])
{
	for(int i=0; i<23; i++)
		for(int j=0; j<14; j++)
			dest[i][j] = source[i][j];
}

Data_2* VirtualMachine::getOptimum(Data_2 *temp)	// 최적의 위치를 구해서 전송해준다.
{
	bool first = true;
	int optimum_x=0;
	int optimum_y=0;
	int optimum_block_state=0;
	double optimum_fx;

	for(int _state=0; _state<4; _state++)
	{
		for(int _x=-1; _x<=8; _x++)
		{
			copyMap(_virtual_map, temp->_map);

			if(check(_x, 0, temp->_block_type, _state)) 
			{
				double fx;	int y;
				for(int _y=0; ; _y++)
					if(!check(_x, _y+1, temp->_block_type, _state)) {
						y = _y;
						break;
					}

					setBlockToMap(_x, y, temp->_block_type, _state, -2);
					fx = getFx();

					if(first || optimum_fx < fx) {
						optimum_x = _x;
						optimum_y = y;
						optimum_block_state = _state;
						optimum_fx = fx;
						first = false;
					}
			}
		}
	}
	temp->_x = optimum_x;
	temp->_y = optimum_y;
	temp->_block_state = optimum_block_state;
	return temp;
}

bool VirtualMachine::check(int x, int y, int block_type, int block_state)	// 충돌 판정 함수
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j] && ((_virtual_map[y+i+1][x+j+2]>=1 && _virtual_map[y+i+1][x+j+2]<20)))
				return false;
	return true;	
}

double VirtualMachine::getFx()	// Fx를 구해주는 함수
{
	int A_count = 0, B_count = 0, C_count = 0, D_count = 0, E_count = 0, F_count = 0, G_count = 0;

	for(int i=0; i<10; i++) {
		for(int j=0; j<20; j++)	{	
			if((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2) {
				A_count += 20 - j;
				break;
			}
		}	
	}
	for(int i=0; i<10; i++)	{
		bool block = false;	bool blank = false;
		for(int j=0; j<20; j++)	{
			if(((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2) && !block)		block = true;
			if(!((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2) && block)		B_count++;		
			if(!((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2) && !blank)		blank = true;
			if(((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2) && blank)		C_count++;
		}	
	}

	for(int j=0; j<20; j++)
	{
		int block_count = 0;

		for(int i=0; i<10; i++)		
			if((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) || _virtual_map[j+1][i+2]==-2)
				block_count++;

		if(block_count == 10)			
			D_count++;
	}

	for(int i=0; i<10; i++)
		for(int j=0; j<20; j++)
			if((_virtual_map[j+1][i+2]>=1 && _virtual_map[j+1][i+2]<20) && ((_virtual_map[j][i+2]==-2) || (_virtual_map[j+2][i+2]==-2) || (_virtual_map[j+1][i+1]==-2) || (_virtual_map[j+1][i+3]==-2)))
				E_count++;

	for(int j=0; j<=19; j++)	if(_virtual_map[j+1][2]==-2)	F_count++;
	for(int j=0; j<=19; j++)	if(_virtual_map[j+1][11]==-2)	F_count++;
	for(int i=0; i<10; i++)		if(_virtual_map[20][i+2]==-2)	G_count++;
	return (A_count*-3.78) + (B_count*-8.8) + (C_count*-0.59) + (D_count*8.2) + (E_count*3.7) + (F_count*2.5) + (G_count*4.0);
}