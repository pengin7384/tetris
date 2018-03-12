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
		cout << ("������ WSAStartup() ����");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	char option = 1;
	setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));	// ���� ����� �ɼ�

	if(hServSock==INVALID_SOCKET)
		cout << ("socket() ����");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port = htons(1234);
	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
		cout << ("bind() ����");
	if(listen(hServSock, 4)==SOCKET_ERROR)
		cout << ("listen() ����");	
	/////////////////////////////////////////////////////////////////////// ������ �ּ� ����� http://forums.codeguru.com/showthread.php?233261-Visual-C-Network-How-to-get-the-local-IP-address(es) �� �����Ͽ����ϴ�.
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

	////////////////////////////////////////////////////////////////////////// ���� ���� ���� (�κ񿡼� ���Ӱ� ��������� ���ÿ����ֱ����� �κ񿡼���_Thread2 �����带 2�� ����Ѵ�.)
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
	////////////////////////////////////////////////////////////////////////////////// �κ�
	int strLen;
	char message[10000];
	Data_0 *D; // ���� ���� �����Ͱ� �������������� Ÿ���� Ȯ���ϱ�����
	Data_3 *D3;
	Data_4 D4; // �÷��̾� ����


	D4._type = 4;
	for(int i=0; i<4; i++) // ���� �ʱ�ȭ ����
	{
		memset(D4._id[i], 0, 50);
		D4._player_exit[i] = false;
	}

	char *test = "a";

	while(!game_start) // ���̵� �����͸� �ް� ���, ���� ���θ� �ľ�
	{

		for(int i=4-1; i>=0; i--)
		{
			if(send(hSocket[i], test, sizeof(test), 0)==-1) // ������ ���������
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

				if(D->_type == DATA_ID)	// ���ŵ� �޽����� ���̵��� ���
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
						if(player_exit[j]) // �����̸� �����ϰ� ���ӵ� ����̿��� ���̵� �˷��ش�.
							send(hSocket[j], (char*)temp, sizeof(*temp), 0);			
				}
			}
		}
		delay(50);
	}
	////////////////////////////////////////////////////////////////////// ���ӽ��� ����
	_Thread2 = false;			// ���̻� ���ӿ�û�� ���� �ʿ䰡 �����Ƿ� ������2�� ��Ȱ��ȭ
	CloseHandle(hThread2);

	Data_5 D5;
	D5._type = 5;
	for(int j=0; j<4; j++)	
	{
		D5._player_number = j;
		delay(100);
		send(hSocket[j], (char*)&D5, sizeof(D5), 0);	// ���ӽ��� 		
	}
	///////////////////////////////////////////////////////////////////// ���ӽ��� (������ ���� �� �й�)
	Data_1 *D1;
	Data_6 *D6;
	ULONG iMode = 1;							
	ioctlsocket( hServSock, FIONBIO, &iMode );	// ������ ��, ���� ���� ������������ ����ŷ���� ��ȯ

	for(int i=0; i<4; i++)
		ioctlsocket( hSocket[i], FIONBIO, &iMode );	// Ŭ���̾�Ʈ���� �Ҵ��� ���Ͽ��� ����ŷ���� ��ȯ

	while(_Thread)
	{
		/////////////////////////////////////////////////////////////////// �й踦 ������ �ο��� �˻�
		int count = 0, index;
		for(int j=0; j<4; j++)
		{
			if(!player_lose[j])
			{
				index = j;
				count++;
			}
		}

		if(count == 1)	// ��������� �Ѹ���ϰ�� �¸��޽����� �����ش�.
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
		//////////////////////////////////////////////////////////////////// ������ �й� �� ó��
		for(int i=0; player_exit[i] && i<4; i++)
		{
			strLen = recv(hSocket[i], message, sizeof(message)-1, 0);
			if(strLen == -1)
				continue;
			message[strLen] = '\0';
			D = (Data_0*)message;

			if(D->_type == 1)		// �÷��̾��� �����Ͱ� ���ð��
			{
				D1 = (Data_1*)message;
				Data_1 *temp = (Data_1*)malloc(sizeof(Data_1));
				memcpy(temp, D1, sizeof(Data_1));
				for(int j=0; j<4; j++)
					if(player_exit[j] && i!=j)		// �����̸� ����, �����͸� ����
						send(hSocket[j], (char*)temp, sizeof(*temp), 0);
				free(temp);
			}
			else if(D->_type == 6)	// �÷��̾��� ���� �����Ͱ� ���ð��
			{
				D6 = (Data_6*)message;
				Data_6 *temp = (Data_6*)malloc(sizeof(Data_6));
				memcpy(temp, D6, sizeof(Data_6));
				delay(100);
				for(int j=0; j<4; j++)
					if(player_exit[j] && i!=j && !player_lose[j])		// �����̸� ����, �����͸� ����
						send(hSocket[j], (char*)temp, sizeof(*temp), 0);			
				free(temp);
			}
			else if(D->_type == 8)	// ���ٴ� �޽����� �� ��� �ش��÷��̾ �й���·�
				player_lose[i] = true;
		}	
		///////////////////////////////////////////////////////////////////////////////
	}
	return 1;
}

DWORD Server::Thread2_Main()	// ���Ʈ ������
{
	int szClntAddr=sizeof(clntAddr);
	while(_Thread2)	 // ������ �޴´�
	{
		if(hSocket[index] == INVALID_SOCKET)
			error("accept() ����");	
		delay(100);
		hSocket[index] = accept(hServSock, (SOCKADDR*)&clntAddr[index], &szClntAddr);
	}
	return 1;
}

