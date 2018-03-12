#include "Control.h"

Control::Control(){
	srand((unsigned)time(NULL));
	_setcursortype(1);
	settitle("Tetris");
	system("color 0f"); 
	system("mode con:cols=92 lines=26"); 
	_Thread2 = true;
	hThread2 = CreateThread(0,0,thread2, static_cast<void*>( this ),0,&ThreadID2);	// 노래 ON 
}

Control::~Control(){
	_Thread2 = false;
	CloseHandle(hThread2);
}

void Control::reset() {
	ULONG iMode = 0;
	if(!hSocket)
		ioctlsocket(hSocket, FIONBIO, &iMode);	// 블로킹 모드로 전환
	Serv.shutdown();
	View.resetMultiView();
	Player_Map.resetMap();
	Computer_Map.resetMap();
	resetControl();	
	menu();
}

void Control::resetControl()	{
	_Thread = false;
	if(!hThread)
		CloseHandle(hThread);
	if(!hSocket)
		closesocket(hSocket);
	WSACleanup();
}

void Control::lose(bool multi)	{
	if(!multi)
	{
		if(MessageBox(NULL, L"패배하였습니다. 관전하시겠습니까?", L"안내", MB_ICONINFORMATION | MB_YESNO) == IDNO)
			reset();
	}
	else
		MessageBox(NULL, L"패배하였습니다. 게임이 끝날때까지 종료하지말아주세요", L"안내", MB_ICONINFORMATION);
}

void Control::win()	{
	MessageBox(NULL, L"승리하였습니다!", L"안내", MB_ICONINFORMATION);
}

void Control::__exit()	
{
	if(MessageBox(NULL, L"초기화면으로 돌아가시겠습니까?", L"안내", MB_ICONINFORMATION | MB_YESNO) == IDYES)
		reset();
	else	{
		MessageBox(NULL, L"Bye", L"안내", MB_ICONINFORMATION);
		exit(1);
	}
}

void Control::MouseClick(int *x, int *y)	{
	HANDLE	hIn, hOut;
	DWORD	dwNOER, defaultMode;
	INPUT_RECORD rec;
	hIn=GetStdHandle(STD_INPUT_HANDLE);
	hOut=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hIn,&defaultMode);
	SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	ReadConsoleInput(hIn,&rec,1,&dwNOER);	// 마우스 두 번 눌림 방지
	delay(100);

	while(1) {
		ReadConsoleInput(hIn,&rec,1,&dwNOER);
		if( rec.EventType == MOUSE_EVENT ){
			if( rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ){
				*x = rec.Event.MouseEvent.dwMousePosition.X;
				*y = rec.Event.MouseEvent.dwMousePosition.Y;
				SetConsoleMode(hIn, defaultMode);
				return;
			}
		} 
	} 
}

void Control::menu()
{
	int x=0, y=0, menu_state=0;
	View.intro();

	while(menu_state != -1)
	{	
		MouseClick(&x, &y);		
		if(menu_state==0)	// 싱글, 멀티
		{
			if(clickLeftButton(x, y))			{	View.menu_1();	menu_state = 1;	}
			else if(clickRightButton(x, y))		{	View.menu_2();	menu_state = 2;	}
		}
		else if(menu_state==1)	// 혼자, 컴퓨터
		{
			if(clickLeftButton(x, y))			{	View.menu_3();	menu_state = 3;	}
			else if(clickRightButton(x, y))		{	View.menu_4();	menu_state = 4;	}
		}
		else if(menu_state==2)	// 서버, 클라이언트
		{
			if(clickLeftButton(x, y))			{	multi_server();	menu_state = -1;	}
			else if(clickRightButton(x, y))		{	multi_client();	menu_state = -1;	}
		}
		else if(menu_state==3)	// 트레이닝, 노말
		{
			if(clickLeftButton(x, y))			{	single(true);	menu_state = -1;	}
			else if(clickRightButton(x, y))		{	single(false);	menu_state = -1;	}
		}
		else if(menu_state==4)	// 초급, 중급, 고급, ??
		{	
			if(click1Button(x, y))				{	single(false, true, EASY);		menu_state = -1;	}
			else if(click2Button(x, y))			{	single(false, true, NORMAL);	menu_state = -1;	}
			else if(click3Button(x, y))			{	single(false, true, HARD);		menu_state = -1;	}
			else if(click4Button(x, y))			{	single(false, true, EXTREME);	menu_state = -1;	}	
		}
		else if(menu_state==-1)
			break;			
	}
}

void Control::downTimer(int &down_timer)	// 내려가는 시간 (점점 빨라짐)
{
	static int count=0;
	if(count++ > 1000 && down_timer > 25){
		down_timer -= 10;
		count = 0;
	}
}

void Control::single(bool assistant, bool computer, int level)	// 싱글 통합 (도우미, 컴퓨터, 난이도)
{		
	View.basic(1+computer);
	View.startThread_MuliView();
	if(!computer)
		View.drawScore();

	int timer=0, computer_timer=0, computer_move_timer=0, down_timer=100, player=0, player_clear=0, computer_clear=0, score=0, total_line=0;
	bool spacebar = false;

	if(level == EASY)			level = 30;	// 컴퓨터 무브 딜레이 설정
	else if(level == NORMAL)	level = 20;
	else if(level == HARD)		level = -1;
	else if(level == EXTREME)	{	
		level = -1; 
		spacebar = true; 
	}

	while(1)
	{
		if(!Player_Map.isBlockExit())				// 블럭 X
		{
			clear(computer, player_clear);			// 라인 정리

			if(!Player_Map.createNewBlock())		// 새로운 블럭 생성이 불가능
			{
				Player_Map.eraseGoodPos();			// 도우미 경로 지우기
				Player_Map.die();					// 사망
				View.requestMapView(Player_Map.getData(player));
				__exit();
				break;
			}
			if(assistant)	// 도우미가 켜져있을경우 최적의 경로 출력
				Player_Map.showGoodPos(Virtual.getOptimum(Player_Map.getData()));
			View.requestMapView(Player_Map.getData(player));
		}

		if(!Computer_Map.isBlockExit() && computer)	// 컴퓨터 블럭 X
		{
			computer_clear += Computer_Map.lineClear();

			if(!Computer_Map.createNewBlock())		// 컴퓨터 새로운 블럭 생성이 불가능
			{
				Computer_Map.die();
				View.requestMapView(Computer_Map.getData(computer));
				win();
				__exit();
				break;
			}
		}

		if(_kbhit())	// 키보드 입력
			keyEvent(player, timer, assistant);

		if(computer_move_timer++ > level && computer)	{	// 컴퓨터 이동 딜레이 
			if(Computer_Map.movePos(Virtual.getOptimum(Computer_Map.getData()), spacebar) == 2) // 컴퓨터 최적의 경로를 받아서 움직인다.
				computer_timer = 101;	// 원하는 위치일경우 그자리에 고정시킨다.
			View.requestMapView(Computer_Map.getData(computer));
			computer_move_timer = 0;
		}

		if(timer++ > down_timer)	{ 			// 일정 시간이 지나면 플레이어 블럭 한칸 내리기
			Player_Map.blockDown();
			View.requestMapView(Player_Map.getData(player));
			timer = 0;
		}

		if(computer_timer++ > down_timer && computer)	{ 			// 일정 시간이 지나면 컴퓨터 블럭 한칸 내리기
			Computer_Map.blockDown();
			View.requestMapView(Computer_Map.getData(computer));
			computer_timer = 0;
		}

		if(player_clear >= 3 && computer) {	// 플레이어가 3라인이상 클리어했을경우 컴퓨터에게 공격
			Computer_Map.recvLineAttack();
			View.requestMapView(Computer_Map.getData(computer));
			player_clear -= 3;
		}

		if(computer_clear >= 3 && computer)	{	// 컴퓨터가 3라인이상 클리어했을경우 플레이어에게 공격
			Player_Map.recvLineAttack();
			View.requestMapView(Player_Map.getData(player));
			computer_clear -= 3;
		}

		downTimer(down_timer);	// 타이머 함수
		delay(10);
	}
}

void Control::clear(bool computer, int &player_clear)
{
	if(!computer)	{	// 컴퓨터가 없을경우 점수시스템
		int count = Player_Map.lineClear();;
		player_clear += count;
		View.sendScore(count);
	}
	else
		player_clear += Player_Map.lineClear();
}

void Control::keyEvent(int player, int &timer, bool assistant)	// 키보드 입력 처리
{
	switch(_getch())
	{	
	case Z:	// 홀드
		{
			Player_Map.holdBlock();	
			if(assistant)
				Player_Map.showGoodPos(Virtual.getOptimum(Player_Map.getData()));	// 블럭을 바꿨으므로 최적의 경로또한 바꿔준다.
			break;
		}
	case LEFT:	Player_Map.moveBlock(-1, 0);					break;	// 좌
	case RIGHT:	Player_Map.moveBlock(+1, 0);				break;	// 우
	case UP:	Player_Map.rotationBlock();					break;	// 회전
	case DOWN:	Player_Map.moveBlock(0, +1);				break;	// 한칸 내리기
	case SPACE: Player_Map.loopDownBlock();	timer = 100;	break;	// 쭉 내리기
	}
	View.requestMapView(Player_Map.getData(player));
}

void Control::multi_server()
{
	Serv.start();		// 서버유저로 접속할경우 서버를 오픈한다.
	multi_client(true);	// 서버를 연후에 서버유저로서 접속
}

void Control::error(char str[])	// 예외 오류 처리
{
	cout << str;
	exit(1);
}

void Control::multi_client(bool server)		// 클라이언트 유저 접속 (서버유저인지 아닌지)
{
	char message[2000] = {0};
	int strLen;
	system("mode con:cols=50 lines=20"); 
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) !=0)
		error("WSAStartup() 오류");
	hSocket=socket(PF_INET, SOCK_STREAM, 0);

	if(hSocket==INVALID_SOCKET)
		error("socket() 오류");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;

	Data_3 *D3 = (Data_3*)malloc(sizeof(Data_3));

	char str[50];
	do	{
		cout << "아이디 입력 (최대 8글자) : ";
		cin >> str;		
	} while(strlen(str)>16);
	memcpy(D3->_id, str, sizeof(D3->_id));
	D3->_type = 3;

	if(!server) // 일반 클라이언트 유저일경우 아이피 입력
	{
		char ip[20];
		cout << "서버 아이피 입력 : ";
		cin >> ip;
		servAddr.sin_addr.s_addr=inet_addr(ip);
	}
	else	// 서버 유저일경우 자신의 로컬아이피로 접속
		servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	servAddr.sin_port=htons(1234);
	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		error("connect() 오류");
	send(hSocket, (char*)D3, sizeof(*D3), 0);

	Data_0 *D; 
	Data_4 *D4;
	Data_4 *Player_Data = (Data_4*)malloc(sizeof(Data_4));	// 플레이어들의 데이터 백업

	View.serverState();

	while(1)	// 현재 상황, 아이디를 받아서 반영
	{
		if(_kbhit())	// 명령어 입력
		{
			gotoxy(4, 10);
			char order[100];
			cin >> order;
			if(strcmp(order, ":start") == 0)
				Serv.letsStart();
			gotoxy(4, 10); cout << "                                 ";
		}

		strLen = recv(hSocket, message, sizeof(message)-1, 0);
		if(strLen == -1)
			continue;
		message[strLen] = '\0';			
		D = (Data_0*)message;			// 메시지를 받아서 Data_0으로 형변환 (어떤 타입의 데이터인지 알기위해)

		if(D->_type == 4)	// 정보 업데이트
		{
			D4 = (Data_4*)message;	
			gotoxy(4, 3);	printf("Server IP : %s",D4->_ip);

			for(int i=0; i<4; i++)	{			
				gotoxy(4, 5+i); printf("Player (%d) : %-20s", i+1, D4->_id[i]);			
				memcpy(Player_Data, D4, sizeof(Data_4));
			}
		}
		else if(D->_type == 5)	// 게임시작
		{
			Data_5 *D5 = (Data_5*)message;
			multi(D5->_player_number, Player_Data);
			break;
		}
		delay(10);
	}
}

void Control::multi(int player, Data_4 *Player_Data)
{		
	bool first = true;
	View.basic(1+3);
	View.printID(Player_Data);
	View.startThread_MuliView();
	_Thread = true;
	hThread = CreateThread(0,0,thread, static_cast<void*>( this ),0,&ThreadID);
	int timer=0, player_clear=0, down_timer=100;

	while(1)
	{
		if(!Player_Map.isBlockExit())				// 블럭 X
		{
			player_clear += Player_Map.lineClear();
			if(first) {								// 플레이어 접속 대기
				delay(3000);	sendData(Player_Map.getData(player));
				View.requestMapView(Player_Map.getData(player));
				delay(3000);	sendData(Player_Map.getData(player));
				delay(3000);
				first = false;
			}		  

			if(!Player_Map.createNewBlock())		// 새로운 블럭 생성이 불가능
			{
				Player_Map.die();
				delay(100);
				sendData(Player_Map.getData(player));	
				View.requestMapView(Player_Map.getData(player));
				delay(100);
				sendLose();	// 패배했다고 서버에게 알려준다.
				lose(true);
				break;
			}
			View.requestMapView( Player_Map.getData(player));
			sendData(Player_Map.getData(player));	// 서버에게 맵정보 업데이트
		}

		if(_kbhit())	// 키보드 입력
		{
			keyEvent(player, timer);
			sendData(Player_Map.getData(player));
		}

		if(timer++ > down_timer)	{ 			// 일정 시간이 지나면 플레이어 블럭 한칸 내리기
			Player_Map.blockDown();
			View.requestMapView(Player_Map.getData(player));
			sendData(Player_Map.getData(player));
			timer = 0;
		}

		if(player_clear >= 3)	{	// 일정 라인을 클리어할 경우 서버에게 공격메시지를 보낸다. (서버는 다른유저에게 메시지를 뿌려준다.)
			sendAttck(player);
			player_clear -= 3;				
		}

		downTimer(down_timer);
		delay(10);
	}
}

int Control::sendData(Data_1 *temp) {	// 데이터 전송 함수
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

int Control::sendAttck(int player) {	// 공격 메시지 함수
	Data_6 *temp = (Data_6*)malloc(sizeof(Data_6));
	temp->_type = 6;
	temp->_player = player;
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

int Control::sendLose() {				// 패배 메시지
	Data_8 *temp = (Data_8*)malloc(sizeof(Data_8));
	temp->_type = 8;
	delay(100);
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

DWORD WINAPI Control::thread( void* arg )	{	// 쓰레드를 전역>멤버로 캐스팅
	Control *ThisThread= static_cast< Control* >( arg );
	ThisThread->Thread_Main();
	return 1;
}

DWORD WINAPI Control::thread2( void* arg )	{	// 쓰레드를 전역>멤버로 캐스팅
	Control *ThisThread= static_cast< Control* >( arg );
	ThisThread->Thread2_Main();
	return 1;
}

DWORD Control::Thread2_Main()	// 노래재생
{
	while(_Thread2)	{ 
		PlaySound(L"bgm2.wav", NULL, SND_ASYNC); 
		delay(1000*60*5);	
	}
	return 1;
}

DWORD Control::Thread_Main()	// 소켓 데이터 받기
{
	Data_0 *D;		
	Data_1 *D1;
	Data_6 *D6;

	int strLen;
	char message[10000];

	ULONG iMode = 1;
	ioctlsocket( hSocket, FIONBIO, &iMode );	// 논블로킹 모드로 전환

	while(_Thread)
	{
		delay(10);
		strLen = recv(hSocket, message, sizeof(message)-1, 0);	// 서버로부터 데이터 수신
		if(strLen == -1)
			continue;
		message[strLen] = '\0';	
		D = (Data_0*)message;

		if(D->_type == 1)	// 플레이어 정보 데이터를 수신할경우
		{
			D1 = (Data_1*)message;
			Data_1 *temp = (Data_1*)malloc(sizeof(Data_1));
			memcpy(temp, D1, sizeof(Data_1));
			View.requestMapView(temp);		// 해당 플레이어의 정보를 업데이트해준다.
		}
		else if(D->_type == 6)	// 공격메시지를 수신
		{
			delay(100);
			D6 = (Data_6*)message;
			Player_Map.recvLineAttack();	// 공격을 받음
		}	
		else if(D->_type == 7)	// 승리메시지를 수신
			win();				
		else if(D->_type == 9)	// 멀티 종료메시지를 수신
			__exit();		
	}
	return 1;
}

bool Control::clickLeftButton(int x, int y)	{	// 2칸 왼쪽
	if(x>=12 && x<=35 && y==15 )
		return true;
	return false;
}

bool Control::clickRightButton(int x, int y) {	// 2칸 오른쪽
	if(x>=57 && x<=80 && y==15 )
		return true;
	return false;
}

bool Control::click1Button(int x, int y)	{	// 4칸 1
	if(x>=12 && x<=24 && y==15 )
		return true;
	return false;
}

bool Control::click2Button(int x, int y)	{	// 4칸 2
	if(x>=31 && x<=42 && y==15 )
		return true;
	return false;
}

bool Control::click3Button(int x, int y)	{	// 4칸 1
	if(x>=50 && x<=62 && y==15 )
		return true;
	return false;
}

bool Control::click4Button(int x, int y)	{	// 4칸 1
	if(x>=70 && x<=82 && y==15 )
		return true;
	return false;
}
