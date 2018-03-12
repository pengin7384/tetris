#include "Map.h"

Map::Map() { 
	resetMap();
}
Map::~Map() { }

void Map::resetMap()
{
	for(int i=0; i<23; i++)
		for(int j=0; j<14; j++)
			_map[i][j] = 9;

	for(int i=0; i<=20; i++)
		for(int j=2; j<=11; j++)
			_map[i][j] = 0;
	_score = 0;
	_prev_block_type = rand()%7;
	_block_type = rand()%7;
	_block_state = 0;
	_block_exit = false;
	_hold_block = 10;
}

void Map::setBlockToMap(int x, int y, int block_type, int block_state, int what)	// �� (x,y)�� ?? �� ����
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j])
				_map[y+i+1][x+j+2] = what;
}

/*
void Map::setHoldBlock(int hold_block)	{
	_hold_block = hold_block;
}

void Map::setBlockType(int block_type)	{
	_block_type = block_type;
}

void Map::setBlockState(int block_state){
	_block_state = block_state;
}*/

void Map::lineAttack(int count)	// ���ΰ����� �޾������ (���)
{
	if(count <= 0)
		return;
	for(int i=0; i<20; i++)
		for(int j=0; j<10; j++)
			_map[i][j+2] = _map[i+1][j+2];
	for(int j=0; j<10; j++)
		_map[20][j+2] = 7;
	_map[20][rand()%10 + 2] = 0;
	lineAttack(count-1);
}

void Map::eraseGoodPos()	// ������� ����
{
	for(int i=0; i<20; i++)
		for(int j=0; j<10; j++)
			if(_map[i+1][j+2] <= -20 && _map[i+1][j+2] > -30)
				_map[i+1][j+2] = 0;
}

void Map::showGoodPos(Data_2 *temp)	// ������� �ݿ�
{
	eraseGoodPos();
	setBlockToMap(temp->_x, temp->_y, temp->_block_type, temp->_block_state, -(temp->_block_type)-20);
}

void Map::updateExPath() 
{
	for(int y=_y; ; y++)
		if(!check(_x, y+1, _block_type, _block_state))
		{
			setBlockToMap(_x, y, _block_type, _block_state, -(_block_type)-10);
			break;
		}		
}

void Map::eraseExPath(int x, int block_type, int block_state) 
{ 
	for(int i=_y; ; i++)
		if(!check(x, i+1, _block_type, _block_state))
		{
			setBlockToMap(x, i, _block_type, _block_state, 0);
			break;
		}	
}

void Map::blockDown()	// ���� ��ĭ �����ش�
{
	if(!check(_x, _y+1, _block_type, _block_state))	{ // �Ʒ��� ���� ��쿡�� �״´�
		setBlockToMap(_x, _y, _block_type, _block_state, _block_type+10);			
		_block_exit = false;
	}
	else	{
		setBlockToMap(_x, _y, _block_type, _block_state, 0);
		_y++;
		setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);					
	}
}

void Map::die()	// �׾����� Ư��ȿ�� (���)
{
	setBlockToMap(_x, _y, _block_type, _block_state, _block_type+10);	
	for(int i=0; i<20; i++)	
		for(int j=0; j<10; j++)
			if(_map[i+1][j+2]>=1 && _map[i+1][j+2]<20)
				_map[i+1][j+2] = 7;
}

void Map::recvLineAttack()	// ������ �޾������ �Ʒ��� �� ĭ�� �� �����߰�
{
	setBlockToMap(_x, _y, _block_type, _block_state, 0);
	if(_y >= 0)
		_y--;
	for(int i=0; i<20; i++)
		for(int j=0; j<10; j++)
			_map[i][j+2] = _map[i+1][j+2];

	for(int j=0; j<10; j++)
		_map[20][j+2] = 7;
	_map[20][rand()%10 + 2] = 0;
	setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);			
}

void Map::moveBlock(int move_x, int move_y)	// �� �̵�
{
	if(check(_x+move_x , _y+move_y , _block_type, _block_state))
	{
		eraseExPath(_x, _block_type, _block_state);
		setBlockToMap(_x , _y, _block_type, _block_state, 0);
		_x += move_x;
		_y += move_y;
		updateExPath();
		setBlockToMap(_x , _y, _block_type, _block_state, _block_type+20);
	}
}

void Map::loopDownBlock()	// '�����̽���'Ű �� ������
{
	int y=_y;
	setBlockToMap(_x , _y, _block_type, _block_state, 0);
	for(; ; y++)
		if(!check(_x, y+1, _block_type, _block_state))
		{
			_y = y;
			break;
		}
		setBlockToMap(_x , _y, _block_type, _block_state, _block_type+20);
}

void Map::rotationBlock()	// ȸ��
{
	int backup_x = _x, vir_block_state = _block_state;
	bool rot=false;
	vir_block_state++;
	if(vir_block_state >= 4)
		vir_block_state = vir_block_state % 4;


	if(check(_x, _y, _block_type, vir_block_state)) // ȸ���� ����
		rot = true;
	else if(!checkWall(_x, _y, _block_type, vir_block_state))	// �������� �Ұ��� (������ ��ĭ �о ȸ)
	{
		if(_x<=2)
		{				
			for(int i=1; !rot && i<=2; i++)
				if(check(_x+i, _y, _block_type, vir_block_state))
				{
					_x += i;	rot = true;
				}
		}
		else
		{
			for(int i=-1; !rot && i>=-2; i--)
				if(check(_x+i, _y, _block_type, vir_block_state))
				{
					_x += i;	rot = true;
				}
		}
	}
	if(rot)	{	// ȸ�� ������� �ݿ�
		eraseExPath(backup_x, _block_type, _block_state);
		setBlockToMap(backup_x , _y, _block_type, _block_state, 0);
		_block_state = vir_block_state;
		updateExPath();
		setBlockToMap(_x , _y, _block_type, _block_state, _block_type+20);
	}
}

bool Map::holdBlock()	// 'Z'Ű ��Ȧ��
{
	if(check(_x, _y, _hold_block, 0))
	{
		int backup = _hold_block;
		eraseExPath(_x, _block_type, _block_state);
		setBlockToMap(_x, _y, _block_type, _block_state, 0);
		_block_state = 0;
		backup = _hold_block;
		_hold_block = _block_type;
		if(backup == 10) // ù ��° Ȧ��� �ܼ� ����
		{
			_block_type = _prev_block_type;
			_prev_block_type = rand()%7;
		}	
		else
			_block_type = backup;
		updateExPath();	
		setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);	
		return true;
	}
	return false;
}

bool Map::createNewBlock()	// ���ο� �� ����
{
	_block_exit = true;
	_x=3, _y=-1;		// ��ǥ�� ���� ����� �Ű��ش�.
	_block_type = _prev_block_type;
	_prev_block_type = rand()%7;
	_block_state = 0;
	if(!check(_x, _y, _block_type, _block_state))	// �������� �Ұ����Ұ��
		return false;
	updateExPath();
	setBlockToMap(_x, _y, _block_type, 0, _block_type+20);
	return true;
}

bool Map::check(int x, int y, int block_type, int block_state)	// �浹 ���� �Լ�
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j] && ((_map[y+i+1][x+j+2]>=1 && _map[y+i+1][x+j+2]<20)))
				return false;
	return true;	
}	

bool Map::checkWall(int x, int y, int block_type, int block_state)	// �� �浹 ���� �Լ�
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j] && (_map[y+i+1][x+j+2]==9))
				return false;
	return true;	
}

bool Map::isBlockExit()
{
	return _block_exit;
}

int Map::lineClear(int line)	// ����Ŭ���� (���׼���)
{
	for(int i=0; i<20; i++)	{
		int count = 0;
		for(int j=0; j<10; j++)
			if(_map[i+1][j+2]>=1 && _map[i+1][j+2]<20)
				count++;
		if(count == 10)	{
			int distance = 19-i;
			for(int w=19; w>=distance+1; w--)
				for(int j=0; j<10; j++)
					_map[w+1-distance][j+2] = _map[w+1-distance-1][j+2];
			for(int j=0; j<10; j++)
				_map[1][j+2] = 0;
			return lineClear(++line);
		}
	}
	return line;
}



int Map::movePos(Data_2* temp, bool _spacebar=false)	// ��ǻ�͸��� ������θ� �޾Ƽ� �̵�
{
	if((_x != temp->_x) || (_y != temp->_y) || (_block_state != temp->_block_state))		// ��ǻ�Ͱ� ���ϴ� ������ ��ĭ �̵� �Ǵ� ȸ��
	{
		int backup_x = _x, backup_y = _y, backup_block_state = _block_state;
		bool move = false, complete = false;

		if(!move && (_block_state != temp->_block_state) && check(_x, _y, _block_type, (_block_state+1)%4))	{	// ȸ�� 
			eraseExPath(backup_x, _block_type, backup_block_state);
			if(_block_state++ >= 4)
				_block_state = _block_state % 4;
			move = true;
		}
		else if(!move && (_x > temp->_x) && check(_x-1, _y, _block_type, _block_state))	{	// ����
			_x--;
			move = true;
		}			
		else if(!move && (_x < temp->_x) && check(_x+1, _y, _block_type, _block_state))	{	// ������
			_x++;
			move = true;
		}		
		else if(!move && (_y < temp->_y) && check(_x, _y+1, _block_type, _block_state))	{	// �Ʒ�
			if(_spacebar == true)	
			{ // ��ǻ�� �����̽��� ��� ����
				for(; ; _y++)	// �ִ��� �Ʒ����� ��������.
					if(!check(_x, _y+1, _block_type, _block_state))
						break;			
			}
			else 
				_y++;	// �����̽��ٰ� ���� ���������
			move = true;			
		}

		if(move){	// ������ �ݿ�			
			eraseExPath(backup_x, _block_type, backup_block_state);
			updateExPath();
			setBlockToMap(backup_x, backup_y, _block_type, backup_block_state, 0);	
			setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);
			if((_x == temp->_x) && (_y == temp->_y) && (_block_state == temp->_block_state))	 // ���ϴ� ��ġ�� ���
				return 2;
			return 1;		
		}
	}
	return 0;	
}

Data_1* Map::getData(int player)	// �ش� �÷��̾��� ������ ����ü�� �����Ͽ� �������ش�.
{
	Data_1 *temp = (Data_1*)malloc(sizeof(Data_1));
	for(int i=0; i<23; i++)
		for(int j=0; j<14; j++)
			temp->_map[i][j] = _map[i][j];
	temp->_type = 1;
	temp->_prev_block_type = _prev_block_type;
	temp->_player = player;
	temp->_hold_block = _hold_block;
	return temp;
}

Data_2* Map::getData()	// �ΰ����ɿ� ������
{
	Data_2 *temp = (Data_2*)malloc(sizeof(Data_2));
	temp->_block_state = _block_state;
	temp->_block_type = _block_type;
	for(int i=0; i<23; i++)
		for(int j=0; j<14; j++)
			temp->_map[i][j] = _map[i][j];
	temp->_x = _x;
	temp->_y = _y;
	return temp;
}
