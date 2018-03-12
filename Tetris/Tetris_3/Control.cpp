#include "Control.h"

Control::Control(){
	srand((unsigned)time(NULL));
	_setcursortype(1);
	settitle("Tetris");
	system("color 0f"); 
	system("mode con:cols=92 lines=26"); 
	_Thread2 = true;
	hThread2 = CreateThread(0,0,thread2, static_cast<void*>( this ),0,&ThreadID2);	// �뷡 ON 
}

Control::~Control(){
	_Thread2 = false;
	CloseHandle(hThread2);
}

void Control::reset() {
	ULONG iMode = 0;
	if(!hSocket)
		ioctlsocket(hSocket, FIONBIO, &iMode);	// ���ŷ ���� ��ȯ
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
		if(MessageBox(NULL, L"�й��Ͽ����ϴ�. �����Ͻðڽ��ϱ�?", L"�ȳ�", MB_ICONINFORMATION | MB_YESNO) == IDNO)
			reset();
	}
	else
		MessageBox(NULL, L"�й��Ͽ����ϴ�. ������ ���������� �������������ּ���", L"�ȳ�", MB_ICONINFORMATION);
}

void Control::win()	{
	MessageBox(NULL, L"�¸��Ͽ����ϴ�!", L"�ȳ�", MB_ICONINFORMATION);
}

void Control::__exit()	
{
	if(MessageBox(NULL, L"�ʱ�ȭ������ ���ư��ðڽ��ϱ�?", L"�ȳ�", MB_ICONINFORMATION | MB_YESNO) == IDYES)
		reset();
	else	{
		MessageBox(NULL, L"Bye", L"�ȳ�", MB_ICONINFORMATION);
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
	ReadConsoleInput(hIn,&rec,1,&dwNOER);	// ���콺 �� �� ���� ����
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
		if(menu_state==0)	// �̱�, ��Ƽ
		{
			if(clickLeftButton(x, y))			{	View.menu_1();	menu_state = 1;	}
			else if(clickRightButton(x, y))		{	View.menu_2();	menu_state = 2;	}
		}
		else if(menu_state==1)	// ȥ��, ��ǻ��
		{
			if(clickLeftButton(x, y))			{	View.menu_3();	menu_state = 3;	}
			else if(clickRightButton(x, y))		{	View.menu_4();	menu_state = 4;	}
		}
		else if(menu_state==2)	// ����, Ŭ���̾�Ʈ
		{
			if(clickLeftButton(x, y))			{	multi_server();	menu_state = -1;	}
			else if(clickRightButton(x, y))		{	multi_client();	menu_state = -1;	}
		}
		else if(menu_state==3)	// Ʈ���̴�, �븻
		{
			if(clickLeftButton(x, y))			{	single(true);	menu_state = -1;	}
			else if(clickRightButton(x, y))		{	single(false);	menu_state = -1;	}
		}
		else if(menu_state==4)	// �ʱ�, �߱�, ���, ??
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

void Control::downTimer(int &down_timer)	// �������� �ð� (���� ������)
{
	static int count=0;
	if(count++ > 1000 && down_timer > 25){
		down_timer -= 10;
		count = 0;
	}
}

void Control::single(bool assistant, bool computer, int level)	// �̱� ���� (�����, ��ǻ��, ���̵�)
{		
	View.basic(1+computer);
	View.startThread_MuliView();
	if(!computer)
		View.drawScore();

	int timer=0, computer_timer=0, computer_move_timer=0, down_timer=100, player=0, player_clear=0, computer_clear=0, score=0, total_line=0;
	bool spacebar = false;

	if(level == EASY)			level = 30;	// ��ǻ�� ���� ������ ����
	else if(level == NORMAL)	level = 20;
	else if(level == HARD)		level = -1;
	else if(level == EXTREME)	{	
		level = -1; 
		spacebar = true; 
	}

	while(1)
	{
		if(!Player_Map.isBlockExit())				// �� X
		{
			clear(computer, player_clear);			// ���� ����

			if(!Player_Map.createNewBlock())		// ���ο� �� ������ �Ұ���
			{
				Player_Map.eraseGoodPos();			// ����� ��� �����
				Player_Map.die();					// ���
				View.requestMapView(Player_Map.getData(player));
				__exit();
				break;
			}
			if(assistant)	// ����̰� ����������� ������ ��� ���
				Player_Map.showGoodPos(Virtual.getOptimum(Player_Map.getData()));
			View.requestMapView(Player_Map.getData(player));
		}

		if(!Computer_Map.isBlockExit() && computer)	// ��ǻ�� �� X
		{
			computer_clear += Computer_Map.lineClear();

			if(!Computer_Map.createNewBlock())		// ��ǻ�� ���ο� �� ������ �Ұ���
			{
				Computer_Map.die();
				View.requestMapView(Computer_Map.getData(computer));
				win();
				__exit();
				break;
			}
		}

		if(_kbhit())	// Ű���� �Է�
			keyEvent(player, timer, assistant);

		if(computer_move_timer++ > level && computer)	{	// ��ǻ�� �̵� ������ 
			if(Computer_Map.movePos(Virtual.getOptimum(Computer_Map.getData()), spacebar) == 2) // ��ǻ�� ������ ��θ� �޾Ƽ� �����δ�.
				computer_timer = 101;	// ���ϴ� ��ġ�ϰ�� ���ڸ��� ������Ų��.
			View.requestMapView(Computer_Map.getData(computer));
			computer_move_timer = 0;
		}

		if(timer++ > down_timer)	{ 			// ���� �ð��� ������ �÷��̾� �� ��ĭ ������
			Player_Map.blockDown();
			View.requestMapView(Player_Map.getData(player));
			timer = 0;
		}

		if(computer_timer++ > down_timer && computer)	{ 			// ���� �ð��� ������ ��ǻ�� �� ��ĭ ������
			Computer_Map.blockDown();
			View.requestMapView(Computer_Map.getData(computer));
			computer_timer = 0;
		}

		if(player_clear >= 3 && computer) {	// �÷��̾ 3�����̻� Ŭ����������� ��ǻ�Ϳ��� ����
			Computer_Map.recvLineAttack();
			View.requestMapView(Computer_Map.getData(computer));
			player_clear -= 3;
		}

		if(computer_clear >= 3 && computer)	{	// ��ǻ�Ͱ� 3�����̻� Ŭ����������� �÷��̾�� ����
			Player_Map.recvLineAttack();
			View.requestMapView(Player_Map.getData(player));
			computer_clear -= 3;
		}

		downTimer(down_timer);	// Ÿ�̸� �Լ�
		delay(10);
	}
}

void Control::clear(bool computer, int &player_clear)
{
	if(!computer)	{	// ��ǻ�Ͱ� ������� �����ý���
		int count = Player_Map.lineClear();;
		player_clear += count;
		View.sendScore(count);
	}
	else
		player_clear += Player_Map.lineClear();
}

void Control::keyEvent(int player, int &timer, bool assistant)	// Ű���� �Է� ó��
{
	switch(_getch())
	{	
	case Z:	// Ȧ��
		{
			Player_Map.holdBlock();	
			if(assistant)
				Player_Map.showGoodPos(Virtual.getOptimum(Player_Map.getData()));	// ���� �ٲ����Ƿ� ������ ��ζ��� �ٲ��ش�.
			break;
		}
	case LEFT:	Player_Map.moveBlock(-1, 0);					break;	// ��
	case RIGHT:	Player_Map.moveBlock(+1, 0);				break;	// ��
	case UP:	Player_Map.rotationBlock();					break;	// ȸ��
	case DOWN:	Player_Map.moveBlock(0, +1);				break;	// ��ĭ ������
	case SPACE: Player_Map.loopDownBlock();	timer = 100;	break;	// �� ������
	}
	View.requestMapView(Player_Map.getData(player));
}

void Control::multi_server()
{
	Serv.start();		// ���������� �����Ұ�� ������ �����Ѵ�.
	multi_client(true);	// ������ ���Ŀ� ���������μ� ����
}

void Control::error(char str[])	// ���� ���� ó��
{
	cout << str;
	exit(1);
}

void Control::multi_client(bool server)		// Ŭ���̾�Ʈ ���� ���� (������������ �ƴ���)
{
	char message[2000] = {0};
	int strLen;
	system("mode con:cols=50 lines=20"); 
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) !=0)
		error("WSAStartup() ����");
	hSocket=socket(PF_INET, SOCK_STREAM, 0);

	if(hSocket==INVALID_SOCKET)
		error("socket() ����");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;

	Data_3 *D3 = (Data_3*)malloc(sizeof(Data_3));

	char str[50];
	do	{
		cout << "���̵� �Է� (�ִ� 8����) : ";
		cin >> str;		
	} while(strlen(str)>16);
	memcpy(D3->_id, str, sizeof(D3->_id));
	D3->_type = 3;

	if(!server) // �Ϲ� Ŭ���̾�Ʈ �����ϰ�� ������ �Է�
	{
		char ip[20];
		cout << "���� ������ �Է� : ";
		cin >> ip;
		servAddr.sin_addr.s_addr=inet_addr(ip);
	}
	else	// ���� �����ϰ�� �ڽ��� ���þ����Ƿ� ����
		servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	servAddr.sin_port=htons(1234);
	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
		error("connect() ����");
	send(hSocket, (char*)D3, sizeof(*D3), 0);

	Data_0 *D; 
	Data_4 *D4;
	Data_4 *Player_Data = (Data_4*)malloc(sizeof(Data_4));	// �÷��̾���� ������ ���

	View.serverState();

	while(1)	// ���� ��Ȳ, ���̵� �޾Ƽ� �ݿ�
	{
		if(_kbhit())	// ��ɾ� �Է�
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
		D = (Data_0*)message;			// �޽����� �޾Ƽ� Data_0���� ����ȯ (� Ÿ���� ���������� �˱�����)

		if(D->_type == 4)	// ���� ������Ʈ
		{
			D4 = (Data_4*)message;	
			gotoxy(4, 3);	printf("Server IP : %s",D4->_ip);

			for(int i=0; i<4; i++)	{			
				gotoxy(4, 5+i); printf("Player (%d) : %-20s", i+1, D4->_id[i]);			
				memcpy(Player_Data, D4, sizeof(Data_4));
			}
		}
		else if(D->_type == 5)	// ���ӽ���
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
		if(!Player_Map.isBlockExit())				// �� X
		{
			player_clear += Player_Map.lineClear();
			if(first) {								// �÷��̾� ���� ���
				delay(3000);	sendData(Player_Map.getData(player));
				View.requestMapView(Player_Map.getData(player));
				delay(3000);	sendData(Player_Map.getData(player));
				delay(3000);
				first = false;
			}		  

			if(!Player_Map.createNewBlock())		// ���ο� �� ������ �Ұ���
			{
				Player_Map.die();
				delay(100);
				sendData(Player_Map.getData(player));	
				View.requestMapView(Player_Map.getData(player));
				delay(100);
				sendLose();	// �й��ߴٰ� �������� �˷��ش�.
				lose(true);
				break;
			}
			View.requestMapView( Player_Map.getData(player));
			sendData(Player_Map.getData(player));	// �������� ������ ������Ʈ
		}

		if(_kbhit())	// Ű���� �Է�
		{
			keyEvent(player, timer);
			sendData(Player_Map.getData(player));
		}

		if(timer++ > down_timer)	{ 			// ���� �ð��� ������ �÷��̾� �� ��ĭ ������
			Player_Map.blockDown();
			View.requestMapView(Player_Map.getData(player));
			sendData(Player_Map.getData(player));
			timer = 0;
		}

		if(player_clear >= 3)	{	// ���� ������ Ŭ������ ��� �������� ���ݸ޽����� ������. (������ �ٸ��������� �޽����� �ѷ��ش�.)
			sendAttck(player);
			player_clear -= 3;				
		}

		downTimer(down_timer);
		delay(10);
	}
}

int Control::sendData(Data_1 *temp) {	// ������ ���� �Լ�
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

int Control::sendAttck(int player) {	// ���� �޽��� �Լ�
	Data_6 *temp = (Data_6*)malloc(sizeof(Data_6));
	temp->_type = 6;
	temp->_player = player;
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

int Control::sendLose() {				// �й� �޽���
	Data_8 *temp = (Data_8*)malloc(sizeof(Data_8));
	temp->_type = 8;
	delay(100);
	return send(hSocket, (char*)temp, sizeof(*temp), 0);
}

DWORD WINAPI Control::thread( void* arg )	{	// �����带 ����>����� ĳ����
	Control *ThisThread= static_cast< Control* >( arg );
	ThisThread->Thread_Main();
	return 1;
}

DWORD WINAPI Control::thread2( void* arg )	{	// �����带 ����>����� ĳ����
	Control *ThisThread= static_cast< Control* >( arg );
	ThisThread->Thread2_Main();
	return 1;
}

DWORD Control::Thread2_Main()	// �뷡���
{
	while(_Thread2)	{ 
		PlaySound(L"bgm2.wav", NULL, SND_ASYNC); 
		delay(1000*60*5);	
	}
	return 1;
}

DWORD Control::Thread_Main()	// ���� ������ �ޱ�
{
	Data_0 *D;		
	Data_1 *D1;
	Data_6 *D6;

	int strLen;
	char message[10000];

	ULONG iMode = 1;
	ioctlsocket( hSocket, FIONBIO, &iMode );	// ����ŷ ���� ��ȯ

	while(_Thread)
	{
		delay(10);
		strLen = recv(hSocket, message, sizeof(message)-1, 0);	// �����κ��� ������ ����
		if(strLen == -1)
			continue;
		message[strLen] = '\0';	
		D = (Data_0*)message;

		if(D->_type == 1)	// �÷��̾� ���� �����͸� �����Ұ��
		{
			D1 = (Data_1*)message;
			Data_1 *temp = (Data_1*)malloc(sizeof(Data_1));
			memcpy(temp, D1, sizeof(Data_1));
			View.requestMapView(temp);		// �ش� �÷��̾��� ������ ������Ʈ���ش�.
		}
		else if(D->_type == 6)	// ���ݸ޽����� ����
		{
			delay(100);
			D6 = (Data_6*)message;
			Player_Map.recvLineAttack();	// ������ ����
		}	
		else if(D->_type == 7)	// �¸��޽����� ����
			win();				
		else if(D->_type == 9)	// ��Ƽ ����޽����� ����
			__exit();		
	}
	return 1;
}

bool Control::clickLeftButton(int x, int y)	{	// 2ĭ ����
	if(x>=12 && x<=35 && y==15 )
		return true;
	return false;
}

bool Control::clickRightButton(int x, int y) {	// 2ĭ ������
	if(x>=57 && x<=80 && y==15 )
		return true;
	return false;
}

bool Control::click1Button(int x, int y)	{	// 4ĭ 1
	if(x>=12 && x<=24 && y==15 )
		return true;
	return false;
}

bool Control::click2Button(int x, int y)	{	// 4ĭ 2
	if(x>=31 && x<=42 && y==15 )
		return true;
	return false;
}

bool Control::click3Button(int x, int y)	{	// 4ĭ 1
	if(x>=50 && x<=62 && y==15 )
		return true;
	return false;
}

bool Control::click4Button(int x, int y)	{	// 4ĭ 1
	if(x>=70 && x<=82 && y==15 )
		return true;
	return false;
}
