#include "Server.h"

Server::Server()
{
	for(int i=0; i<4; i++)
		player_lose[i] = true;
	game_start = false;
}

void Server::error(char str[])
{
	cout << str << endl;
	exit(1);
}

void Server::putsXY(int x, int y, char str[])
{
	gotoxy(x, y);
	cout << str;
}

void Server::start()
{
	if(WSAStartup(MAKEWORD(2, 2), &(wsaData))!=0)
		cout << ("서버측 WSAStartup() 오류");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	char option = 1;
	setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));	// 소켓 재생성 옵션

	if(hServSock==INVALID_SOCKET)
		cout << ("socket() 오류");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port = htons(1234);
	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
		cout << ("bind() 오류");
	if(listen(hServSock, 4)==SOCKET_ERROR)
		cout << ("listen() 오류");	
	/////////////////////////////////////////////////////////////////////// 아이피 주소 출력은 http://forums.codeguru.com/showthread.php?233261-Visual-C-Network-How-to-get-the-local-IP-address(es) 를 참고하였습니다.
	char szHostName[128] = "";
	if(::gethostname(szHostName, sizeof(szHostName)))	{}
	struct sockaddr_in SocketAddress;
	struct hostent     *pHost        = 0;
	pHost = ::gethostbyname(szHostName);
	if(!pHost)	{}
	char aszIPAddresses[16];
	memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[0], pHost->h_length);
	strcpy_s(aszIPAddresses, inet_ntoa(SocketAddress.sin_addr));
	strcpy_s(server_ip, aszIPAddresses);

	////////////////////////////////////////////////////////////////////////// 서버 동작 시작 (로비에서 접속과 정보출력을 동시에해주기위해 로비에서만_Thread2 쓰레드를 2개 사용한다.)
	_Thread = true;
	_Thread2 = true;
	hThread = CreateThread(0,0,thread1, static_cast<void*>( this ),0,&ThreadID);
	hThread2 = CreateThread(0,0,thread2, static_cast<void*>( this ),0,&ThreadID2);
}

void Server::shutdown()
{
	_Thread = false;
	_Thread2 = false;
	if(!hThread)
		CloseHandle(hThread);
	if(!hThread2)
		CloseHandle(hThread2);
	if(!hServSock)
		closesocket(hServSock);
	for(int i=0; i<4; i++)
	{
		player_exit[i] = false;
		player_lose[i] = true;	
	}
	game_start = false;
	memset(server_ip, 0,sizeof(server_ip));
}

int Server::howManyPlayer()
{
	int count=0;
	for(int i=0; i<4; i++)
		if(player_exit[i])
			count++;
	return count;
}

void Server::letsStart()
{
	game_start = true;
}

DWORD WINAPI Server::thread1( void* arg )	{
	Server *ThisThread= static_cast< Server* >( arg );
	ThisThread->Thread1_Main();
	return 1;
}
DWORD WINAPI Server::thread2( void* arg )	{
	Server *ThisThread= static_cast< Server* >( arg );
	ThisThread->Thread2_Main();
	return 1;
}

DWORD Server::Thread1_Main()
{
	////////////////////////////////////////////////////////////////////////////////// 로비
	int strLen;
	char message[10000];
	Data_0 *D; // 지금 들어온 데이터가 무슨데이터인지 타입을 확인하기위한
	Data_3 *D3;
	Data_4 D4; // 플레이어 정보


	D4._type = 4;
	for(int i=0; i<4; i++) // 정보 초기화 진행
	{
		memset(D4._id[i], 0, 50);
		D4._player_exit[i] = false;
	}

	char *test = "a";

	while(!game_start) // 아이디 데이터를 받고 출력, 접속 여부를 파악
	{

		for(int i=4-1; i>=0; i--)
		{
			if(send(hSocket[i], test, sizeof(test), 0)==-1) // 연결이 끊겼을경우
			{
				index = i;
				player_exit[i] = false;	
				player_lose[i] = true;
				D4._player_exit[i] = player_exit[i];
				strcpy_s(D4._id[i], "Disconnected    ");
				Data_4 *temp = (Data_4*)malloc(sizeof(Data_4));
				memcpy(temp, &D4, sizeof(Data_4));
				temp->_type = 4;
				for(int j=0; j<4; j++)
					if(player_exit[j]) 
						send(hSocket[j], (char*)temp, sizeof(*temp), 0);	
				closesocket(hSocket[i]);
			}
			else if(!player_exit[i])
			{
				delay(100);
				strLen = recv(hSocket[i], message, sizeof(message)-1, 0);
				if(strLen == 0)
					continue;
				message[strLen] = '\0';				
				D = (Data_0*)message;

				if(D->_type == DATA_ID)	// 수신된 메시지가 아이디일 경우
				{
					D3 = (Data_3*)message;
					strcpy_s(D4._id[i], D3->_id);
					player_exit[i] = true;
					player_lose[i] = false;
					D4._player_exit[i] = player_exit[i];
					Data_4 *temp = (Data_4*)malloc(sizeof(Data_4));
					strcpy_s(D4._ip, server_ip);
					memcpy(temp, &D4, sizeof(Data_4));
					temp->_type = 4;
					delay(100);
					for(int j=0; j<4; j++)
						if(player_exit[j]) // 보낸이를 포함하고 접속된 모든이에게 아이디를 알려준다.
							send(hSocket[j], (char*)temp, sizeof(*temp), 0);			
				}
			}
		}
		delay(50);
	}
	////////////////////////////////////////////////////////////////////// 게임시작 공지
	_Thread2 = false;			// 더이상 접속요청을 받을 필요가 없으므로 쓰레드2는 비활성화
	CloseHandle(hThread2);

	Data_5 D5;
	D5._type = 5;
	for(int j=0; j<4; j++)	
	{
		D5._player_number = j;
		delay(100);
		send(hSocket[j], (char*)&D5, sizeof(D5), 0);	// 게임시작 		
	}
	///////////////////////////////////////////////////////////////////// 게임시작 (데이터 수신 후 분배)
	Data_1 *D1;
	Data_6 *D6;
	ULONG iMode = 1;							
	ioctlsocket( hServSock, FIONBIO, &iMode );	// 데이터 송, 수신 병목 현상으로인해 논블로킹모드로 전환

	for(int i=0; i<4; i++)
		ioctlsocket( hSocket[i], FIONBIO, &iMode );	// 클라이언트에게 할당한 소켓역시 논블로킹으로 전환

	while(_Thread)
	{
		/////////////////////////////////////////////////////////////////// 패배를 제외한 인원수 검색
		int count = 0, index;
		for(int j=0; j<4; j++)
		{
			if(!player_lose[j])
			{
				index = j;
				count++;
			}
		}

		if(count == 1)	// 남은사람이 한명뿐일경우 승리메시지를 보내준다.
		{
			Data_7 *temp = (Data_7*)malloc(sizeof(Data_7));
			temp->_type = 7;
			delay(100);
			send(hSocket[index], (char*)temp, sizeof(*temp), 0);
			delay(1000);
			Data_9 D9;
			D9._type = 9;
			for(int j=0; j<4; j++)
				send(hSocket[j], (char*)&D9, sizeof(D9), 0);
		}
		//////////////////////////////////////////////////////////////////// 데이터 분배 및 처리
		for(int i=0; player_exit[i] && i<4; i++)
		{
			strLen = recv(hSocket[i], message, sizeof(message)-1, 0);
			if(strLen == -1)
				continue;
			message[strLen] = '\0';
			D = (Data_0*)message;

			if(D->_type == 1)		// 플레이어의 데이터가 들어올경우
			{
				D1 = (Data_1*)message;
				Data_1 *temp = (Data_1*)malloc(sizeof(Data_1));
				memcpy(temp, D1, sizeof(Data_1));
				for(int j=0; j<4; j++)
					if(player_exit[j] && i!=j)		// 보낸이를 제외, 데이터를 전송
						send(hSocket[j], (char*)temp, sizeof(*temp), 0);
				free(temp);
			}
			else if(D->_type == 6)	// 플레이어의 공격 데이터가 들어올경우
			{
				D6 = (Data_6*)message;
				Data_6 *temp = (Data_6*)malloc(sizeof(Data_6));
				memcpy(temp, D6, sizeof(Data_6));
				delay(100);
				for(int j=0; j<4; j++)
					if(player_exit[j] && i!=j && !player_lose[j])		// 보낸이를 제외, 데이터를 전송
						send(hSocket[j], (char*)temp, sizeof(*temp), 0);			
				free(temp);
			}
			else if(D->_type == 8)	// 졌다는 메시지가 올 경우 해당플레이어를 패배상태로
				player_lose[i] = true;
		}	
		///////////////////////////////////////////////////////////////////////////////
	}
	return 1;
}

DWORD Server::Thread2_Main()	// 어셉트 쓰레드
{
	int szClntAddr=sizeof(clntAddr);
	while(_Thread2)	 // 접속을 받는다
	{
		if(hSocket[index] == INVALID_SOCKET)
			error("accept() 오류");	
		delay(100);
		hSocket[index] = accept(hServSock, (SOCKADDR*)&clntAddr[index], &szClntAddr);
	}
	return 1;
}

