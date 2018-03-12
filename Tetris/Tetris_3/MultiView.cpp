#include "MultiView.h"

MultiView::MultiView()
{
	_score = 0;
	for(int i=0; i<4; i++)
	{
		_standard_x[i] = 7 + 44*i;
		_standard_y[i] = 4;
	}
}

MultiView::~MultiView()
{
	stopThread_MuliView();
}

void MultiView::resetMultiView()
{
	_score = 0;
	_Thread_MultiView = false;
	CloseHandle(hThread_MultiView);
}

void MultiView::printEndl(int num) {
	for(int i=0; i<num; i++)
		cout << endl;
}

void MultiView::putsXY(int x,int y,char str[], int color)	{
	textcolor(color);
	gotoxy(x,y);
	cout << str;
}

void MultiView::intro()
{
	textcolor(15);
	system("mode con:cols=92 lines=26"); 
	printEndl(2);
	cout << "      ─────   ┌────     ─────     ┌───        ───     ┌────" << endl;
	cout << "          │       │                 │         │      │        │       │" << endl;
	cout << "          │       │                 │         │      │        │       │" << endl;
	cout << "          │       │────         │         │───          │       └────┐" << endl;
	cout << "          │       │                 │         │      │        │                 │" << endl;
	cout << "          │       │                 │         │      │        │                 │" << endl;
	cout << "          │       └────         │         │      │      ───       ────┘" << endl;
	cout << "                                                                            " << endl;
	printEndl(4);
	cout << "            ┌                        ┐                 ┌                        ┐  " << endl;
	cout << "            │       Single Play      │                 │        Multi Play      │" << endl;
	cout << "            └                        ┘                 └                        ┘" << endl;
	printEndl(4);
	cout << "                                                                     Team : cosmos #2" << endl;
	cout << "                                                                     Seokyeong Univ." << endl;
	cout << "                                                                     2014301082 최상희" << endl;
	/////////////////////////////////////////////////////////////////////// 이미지 출력은 http://crasy.tistory.com/8 를 참고하였습니다.
	HWND hWnd = FindWindow(L"ConsoleWindowClass",NULL);
	HBITMAP hImage,hOldBitmap;
	HDC hdc    = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hdc);
	hImage = (HBITMAP) LoadImage(NULL,TEXT("img.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	hOldBitmap = (HBITMAP) SelectObject(hMemDC,hImage);;
	BitBlt(hdc,552,310,400,200,hMemDC,0,0,SRCCOPY);		// 이미지 로드
	SelectObject(hMemDC,hOldBitmap);
	DeleteObject(hImage);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd,hdc);
	///////////////////////////////////////////////////////////////////////
}

void MultiView::serverState(){
	system("mode con:cols=40 lines=12"); 
	cout << endl;
	cout << " ┌─────────────────┐" << endl;
	cout << " │                                  │" << endl;
	cout << " ├─────────────────┤" << endl;
	cout << " │                                  │" << endl;
	cout << " │                                  │" << endl;
	cout << " │                                  │" << endl;
	cout << " │                                  │" << endl;
	cout << " ├─────────────────┤" << endl;
	cout << " │                                  │" << endl;
	cout << " └─────────────────┘" << endl;

}

void MultiView::drawScore() { // 점수판
	putsXY(_standard_x[0]+25, _standard_y[0]+16, "Score");
	putsXY(_standard_x[0]+24, _standard_y[0]+17, "┌            ┐");
	putsXY(_standard_x[0]+24, _standard_y[0]+19, "└            ┘");
}

void MultiView::sendScore(int line){
	gotoxy(_standard_x[0]+25, _standard_y[0]+18);
	textcolor(15);
	_score += line*line*10000;
	printf("%13d", _score);
}

void MultiView::basic(int num) {
	system("mode con:cols=93 lines=26"); 
	textcolor(15);
	if(num==1)		system("mode con:cols=48 lines=26");	// 플레이어의 수에 맞게 콘솔크기 조정
	else if(num==2)	system("mode con:cols=92 lines=26"); 
	else if(num==3)	system("mode con:cols=136 lines=26"); 
	else if(num==4)	system("mode con:cols=180 lines=26"); 

	for(int i=0; i<num; i++)
	{
		putsXY(5+44*i, 3, "▩▩▩▩▩▩▩▩▩▩▩▩  ┌        ┐");
		putsXY(5+44*i, 4, "▩                    ▩");
		putsXY(5+44*i, 5, "▩                    ▩");
		putsXY(5+44*i, 6, "▩                    ▩");
		putsXY(5+44*i, 7, "▩                    ▩");
		putsXY(5+44*i, 8, "▩                    ▩  └        ┘");
		putsXY(5+44*i, 9, "▩                    ▩");
		putsXY(5+44*i, 10, "▩                    ▩   Hold");
		putsXY(5+44*i, 11, "▩                    ▩  ┌        ┐");
		putsXY(5+44*i, 12, "▩                    ▩");
		putsXY(5+44*i, 13, "▩                    ▩");
		putsXY(5+44*i, 14, "▩                    ▩");
		putsXY(5+44*i, 15, "▩                    ▩");
		putsXY(5+44*i, 16, "▩                    ▩  └        ┘");
		for(int j=0; j<7; j++)
			putsXY(5+44*i, 17+j, "▩                    ▩");
		putsXY(5+44*i, 24, "▩▩▩▩▩▩▩▩▩▩▩▩");
	}
}

void MultiView::drawTwoBox(){
	putsXY(1, 15,"            ┌                        ┐                 ┌                        ┐");
	putsXY(1, 17,"            └                        ┘                 └                        ┘");
}


void MultiView::drawFourBox(){
	putsXY(1, 15,"            ┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐ ");
	putsXY(1, 17,"            └──────┘   └──────┘   └──────┘   └──────┘");
}	

void MultiView::menu_1(){
	drawTwoBox();
	putsXY(1, 16,"            │         Single         │                 │       vs Computer      │");
}
void MultiView::menu_2(){
	drawTwoBox();
	putsXY(1, 16,"            │         Server         │                 │          Client        │");
}
void MultiView::menu_3(){
	drawTwoBox();
	putsXY(1, 16,"            │      Training Mode     │                 │       Normal Mode      │");
}
void MultiView::menu_4(){
	drawFourBox();
	putsXY(1, 16,"            │    Easy    │   │   Normal   │   │    Hard    │   │   Extreme  │");
}

void MultiView::requestMapView(Data_1 *temp) {	// 맵 출력 요청 (원형큐에 삽입해준다.)
	Q.inQueue(temp);
}

void MultiView::printID(Data_4 *temp)	// 멀티 아이디 출력
{
	for(int i=0; i<4; i++)	{
		int s_x = _standard_x[i], s_y = _standard_y[i];
		putsXY(s_x+25, s_y+16, temp->_id[i]);
	}
}

void MultiView::processView()	// 디큐를하여 해당 데이터를 출력
{
	Data_1 *Node = Q.deQueue();
	int s_x = _standard_x[Node->_player], s_y = _standard_y[Node->_player];

	for(int i=0; i<20; i++)
	{
		for(int j=0; j<10; j++)
		{
			if(Node->_map[i+1][j+2] != _old_map[Node->_player][i+1][j+2])	
			{
				if(Node->_map[i+1][j+2]==0)										putsXY(s_x+j*2, s_y+i, "  ");
				else if(Node->_map[i+1][j+2]>=20 && Node->_map[i+1][j+2]<30)		putsXY(s_x+j*2, s_y+i, "□", color_code[Node->_map[i+1][j+2]%10]);		// 현재블럭
				else if(Node->_map[i+1][j+2]>=10 && Node->_map[i+1][j+2]<20)		putsXY(s_x+j*2, s_y+i, "■", color_code[Node->_map[i+1][j+2]%10]);		// 쌓인블럭
				else if(Node->_map[i+1][j+2]==7)									putsXY(s_x+j*2, s_y+i, "■", color_code[Node->_map[i+1][j+2]%10]);		// 공격블럭
				else if(Node->_map[i+1][j+2]<=-10 && Node->_map[i+1][j+2]>-20)	putsXY(s_x+j*2, s_y+i, "▣", color_code[-(Node->_map[i+1][j+2])%10]);	// 예상위치
				else if(Node->_map[i+1][j+2]<=-20 && Node->_map[i+1][j+2]>-30)	putsXY(s_x+j*2, s_y+i, "○", color_code[-(Node->_map[i+1][j+2])%10]);	// 최적위치
			}
		}
	}
	if(Node->_hold_block != 10)	// 첫 홀드 블럭일 경우에는 겹치는부분까지 출력해줘야하므로 따로 처리해준다.
	{
		for(int i=1; i<3; i++)
		{
			for(int j=0; j<4; j++)
			{
				if(tetrimino[Node->_hold_block][0][i][j])				putsXY(s_x+j*2+26, s_y+i+8, "■", color_code[Node->_hold_block]);
				else if(tetrimino[Node->_hold_block][0][i][j] == 0)		putsXY(s_x+j*2+26, s_y+i+8, "  ");
			}
		}
	}
	_old_prev_block_type[Node->_player] = Node->_prev_block_type;

	for(int i=1; i<3; i++)	// 홀드블럭 출력 (화면 깜빡거림을 최소화하기 위해서 겹치는부분은 건들지 않는다.)
	{
		for(int j=0; j<4; j++)
		{
			if(tetrimino[Node->_prev_block_type][0][i][j])				putsXY(s_x+j*2+26, s_y+i, "■",color_code[Node->_prev_block_type]);
			else if(tetrimino[Node->_prev_block_type][0][i][j] == 0)	putsXY(s_x+j*2+26, s_y+i, "  ");
		}
	}
	for(int i=0; i<20; i++)
		for(int j=0; j<10; j++)
			_old_map[Node->_player][i+1][j+2] = Node->_map[i+1][j+2];
	free(Node);
}

void MultiView::startThread_MuliView()	// 멀티뷰 쓰레드 동작 시작
{
	_Thread_MultiView = true;
	hThread_MultiView = CreateThread(0,0,Thread_MultiView, static_cast<void*>( this ),0,&ThreadID);
}

void MultiView::stopThread_MuliView()	// 멀티뷰 쓰레드 동작 중지
{
	CloseHandle(hThread_MultiView);
	_Thread_MultiView = false;	
}

DWORD WINAPI MultiView::Thread_MultiView( void* arg )
{
	MultiView *ThisThread= static_cast< MultiView* >( arg );
	ThisThread->Thread_MultiView_Main();
	return 1;
}

DWORD MultiView::Thread_MultiView_Main()	// 여러쓰레드가 동시에 gotoxy를 하면 오류가 생기므로 별도의 쓰레드를 통해 출력 
{
	while(_Thread_MultiView)
		if(!Q.isEmpty()) // 출력할 데이터가 있을경우 원형큐 데이터를 하나 꺼내어 출력
			processView();
	return 1;
}
