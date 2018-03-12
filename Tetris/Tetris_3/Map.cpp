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

void Map::setBlockToMap(int x, int y, int block_type, int block_state, int what)	// 맵 (x,y)에 ?? 블럭 설정
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

void Map::lineAttack(int count)	// 라인공격을 받았을경우 (재귀)
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

void Map::eraseGoodPos()	// 최적경로 제거
{
	for(int i=0; i<20; i++)
		for(int j=0; j<10; j++)
			if(_map[i+1][j+2] <= -20 && _map[i+1][j+2] > -30)
				_map[i+1][j+2] = 0;
}

void Map::showGoodPos(Data_2 *temp)	// 최적경로 반영
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

void Map::blockDown()	// 블럭을 한칸 내려준다
{
	if(!check(_x, _y+1, _block_type, _block_state))	{ // 아래가 막힌 경우에는 쌓는다
		setBlockToMap(_x, _y, _block_type, _block_state, _block_type+10);			
		_block_exit = false;
	}
	else	{
		setBlockToMap(_x, _y, _block_type, _block_state, 0);
		_y++;
		setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);					
	}
}

void Map::die()	// 죽었을때 특수효과 (흑백)
{
	setBlockToMap(_x, _y, _block_type, _block_state, _block_type+10);	
	for(int i=0; i<20; i++)	
		for(int j=0; j<10; j++)
			if(_map[i+1][j+2]>=1 && _map[i+1][j+2]<20)
				_map[i+1][j+2] = 7;
}

void Map::recvLineAttack()	// 공격을 받았을경우 아래에 한 칸이 빈 라인추가
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

void Map::moveBlock(int move_x, int move_y)	// 블럭 이동
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

void Map::loopDownBlock()	// '스페이스바'키 쭉 내리기
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

void Map::rotationBlock()	// 회전
{
	int backup_x = _x, vir_block_state = _block_state;
	bool rot=false;
	vir_block_state++;
	if(vir_block_state >= 4)
		vir_block_state = vir_block_state % 4;


	if(check(_x, _y, _block_type, vir_block_state)) // 회전이 가능
		rot = true;
	else if(!checkWall(_x, _y, _block_type, vir_block_state))	// 벽때문에 불가능 (옆으로 한칸 밀어서 회)
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
	if(rot)	{	// 회전 했을경우 반영
		eraseExPath(backup_x, _block_type, _block_state);
		setBlockToMap(backup_x , _y, _block_type, _block_state, 0);
		_block_state = vir_block_state;
		updateExPath();
		setBlockToMap(_x , _y, _block_type, _block_state, _block_type+20);
	}
}

bool Map::holdBlock()	// 'Z'키 블럭홀드
{
	if(check(_x, _y, _hold_block, 0))
	{
		int backup = _hold_block;
		eraseExPath(_x, _block_type, _block_state);
		setBlockToMap(_x, _y, _block_type, _block_state, 0);
		_block_state = 0;
		backup = _hold_block;
		_hold_block = _block_type;
		if(backup == 10) // 첫 번째 홀드는 단순 저장
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

bool Map::createNewBlock()	// 새로운 블럭 생성
{
	_block_exit = true;
	_x=3, _y=-1;		// 좌표를 맨위 가운데로 옮겨준다.
	_block_type = _prev_block_type;
	_prev_block_type = rand()%7;
	_block_state = 0;
	if(!check(_x, _y, _block_type, _block_state))	// 블럭생성이 불가능할경우
		return false;
	updateExPath();
	setBlockToMap(_x, _y, _block_type, 0, _block_type+20);
	return true;
}

bool Map::check(int x, int y, int block_type, int block_state)	// 충돌 판정 함수
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			if(tetrimino[block_type][block_state][i][j] && ((_map[y+i+1][x+j+2]>=1 && _map[y+i+1][x+j+2]<20)))
				return false;
	return true;	
}	

bool Map::checkWall(int x, int y, int block_type, int block_state)	// 벽 충돌 판정 함수
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

int Map::lineClear(int line)	// 라인클리어 (버그수정)
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



int Map::movePos(Data_2* temp, bool _spacebar=false)	// 컴퓨터맵의 최적경로를 받아서 이동
{
	if((_x != temp->_x) || (_y != temp->_y) || (_block_state != temp->_block_state))		// 컴퓨터가 원하는 곳으로 한칸 이동 또는 회전
	{
		int backup_x = _x, backup_y = _y, backup_block_state = _block_state;
		bool move = false, complete = false;

		if(!move && (_block_state != temp->_block_state) && check(_x, _y, _block_type, (_block_state+1)%4))	{	// 회전 
			eraseExPath(backup_x, _block_type, backup_block_state);
			if(_block_state++ >= 4)
				_block_state = _block_state % 4;
			move = true;
		}
		else if(!move && (_x > temp->_x) && check(_x-1, _y, _block_type, _block_state))	{	// 왼쪽
			_x--;
			move = true;
		}			
		else if(!move && (_x < temp->_x) && check(_x+1, _y, _block_type, _block_state))	{	// 오른쪽
			_x++;
			move = true;
		}		
		else if(!move && (_y < temp->_y) && check(_x, _y+1, _block_type, _block_state))	{	// 아래
			if(_spacebar == true)	
			{ // 컴퓨터 스페이스바 사용 가능
				for(; ; _y++)	// 최대한 아래까지 내려본다.
					if(!check(_x, _y+1, _block_type, _block_state))
						break;			
			}
			else 
				_y++;	// 스페이스바가 금지 돼있을경우
			move = true;			
		}

		if(move){	// 움직임 반영			
			eraseExPath(backup_x, _block_type, backup_block_state);
			updateExPath();
			setBlockToMap(backup_x, backup_y, _block_type, backup_block_state, 0);	
			setBlockToMap(_x, _y, _block_type, _block_state, _block_type+20);
			if((_x == temp->_x) && (_y == temp->_y) && (_block_state == temp->_block_state))	 // 원하는 위치일 경우
				return 2;
			return 1;		
		}
	}
	return 0;	
}

Data_1* Map::getData(int player)	// 해당 플레이어의 데이터 구조체를 생성하여 리턴해준다.
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

Data_2* Map::getData()	// 인공지능용 데이터
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
