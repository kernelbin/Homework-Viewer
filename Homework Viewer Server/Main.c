#include<Windows.h>
#include<WinSock2.h>
#include"Global.h"
#include<stdio.h>
#pragma comment(lib,"Ws2_32.lib")

#define BUF_LEN 65536

unsigned __stdcall AcceptThread(LPVOID Args);
unsigned __stdcall CompletionPortMain(LPVOID Args);
pCLIENT_LINK CreateClientLink(SOCKET ClientSock);

int main()
{
	InitializeServer();
	ServerListenPort(58322);
	AcceptThread(0);
}

BOOL ServerListenPort(USHORT Port)//����һ���˿ڲ����⿪��
{
	BOOL bRet = FALSE;
	SOCKADDR_IN ServerAddr;
	__try
	{

		PortSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (PortSock == INVALID_SOCKET)
		{
			printf("WSASocket ����ִ��ʧ��,WSAGetLastError() = %d\n", WSAGetLastError());
			//DefServerCallback(MinecraftServer, SM_SERVERWSASOCKERROR, WSAGetLastError(), 0);
			__leave;
		}
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		ServerAddr.sin_port = htons(Port);

		if (bind(PortSock, (const struct sockaddr*) & ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		{
			printf("bind ����ִ��ʧ��,WSAGetLastError() = %d\n", WSAGetLastError());
			//DefServerCallback(MinecraftServer, SM_SERVERBINDERROR, WSAGetLastError(), 0);
			__leave;
		}

		if (listen(PortSock, 1000) == SOCKET_ERROR)
		{
			printf("bind ����ִ��ʧ��,WSAGetLastError() = %d\n", WSAGetLastError());
			//DefServerCallback(MinecraftServer, SM_SERVERBINDERROR, WSAGetLastError(), 0);
			__leave;
		}

		_beginthreadex(NULL, 0, AcceptThread, 0, 0, 0);
		//TODO:���߳��Ƿ������ɹ����м�顣���⣬���ǽ�acceptҲ����IOCP
		bRet = TRUE;
	}
	__finally
	{
		if (!bRet)
		{
			//ʧ��
			if (PortSock)
			{
				closesocket(PortSock);
				PortSock = 0;
			}
		}
	}

	return bRet;
}



BOOL InitializeServer()
{
	BOOL bRet = FALSE;
	WSADATA WSAData;
	__try
	{
		

		//��ʼ�������������
		if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
			__leave;

		//����IOCP�˿�
		hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

		OverlappedWithRecvFin = malloc(sizeof(IOCPMODEPACK));
		OverlappedWithSendFin = malloc(sizeof(IOCPMODEPACK));
		memset(OverlappedWithRecvFin, 0, sizeof(IOCPMODEPACK));
		memset(OverlappedWithSendFin, 0, sizeof(IOCPMODEPACK));
		OverlappedWithRecvFin->SendRecvFin = RECV_FIN;
		OverlappedWithSendFin->SendRecvFin = SEND_FIN;


		//�����ȴ��̳߳�
		SYSTEM_INFO SysInfo;//����ȡ��CPU��������Ϣ
		GetSystemInfo(&SysInfo);

		for (unsigned int i = 0; i < SysInfo.dwNumberOfProcessors; i++)
		{
			_beginthreadex(NULL, 0, CompletionPortMain, (LPVOID)0, 0, NULL);
		}

		bRet = TRUE;
	}
	__finally
	{
	}
	return bRet;
}




unsigned __stdcall CompletionPortMain(LPVOID Args)
{
	DWORD ByteTrans;
	IOCPMODEPACK* ModeInfo;
	pCLIENT_LINK Client;
	DWORD BytesRead;
	DWORD Flags = 0;

	while (1)
	{
		BOOL bOK = GetQueuedCompletionStatus(hCompPort, &ByteTrans, (PULONG_PTR)& Client, (LPOVERLAPPED*)& ModeInfo, INFINITE);
		switch (ModeInfo->SendRecvFin)
		{
		case RECV_FIN:
		{
			if (ByteTrans == 0)
			{
				printf("һ�����ӶϿ�\n");
				DestoryClientLink(Client);
				break;
			}
		}
		break;
		case SEND_FIN:
		{

		}
		break;
		}
	}
}



unsigned __stdcall AcceptThread(LPVOID Args)
{
	while (1)
	{
		SOCKET ClientSock;
		SOCKADDR_IN ClientAddr;
		int AddrLen = sizeof(SOCKADDR_IN);

		ClientSock = accept(PortSock, (struct sockaddr*) & ClientAddr, &AddrLen);

		//pCLIENTINFO_PACK InfoPack = malloc(sizeof(CLIENTINFO_PACK));


		pCLIENT_LINK Client = CreateClientLink(ClientSock);
		
		if (!Client)
		{
			closesocket(ClientSock);
			continue;
		}

		printf("������,�����ַ%lld\n", (__int64)Client);
		//memset(InfoPack, 0, sizeof(CLIENTINFO_PACK));

		//InfoPack->SendRecvFin = RECV_FIN;


		CreateIoCompletionPort((HANDLE)ClientSock, hCompPort, (ULONG_PTR)Client, 0);//TODO: �ò����ĵڶ����������ݵ�ֵ����Completion PortҲ�����յ����������Ǹ�InfopackҲ�ǣ���������ô���ã�



		DWORD BytesRead;
		DWORD Flags = 0;

		WSARecv(ClientSock, &(Client->WSABuf), 1, &BytesRead, &Flags, (LPWSAOVERLAPPED)OverlappedWithRecvFin, NULL);
	}
}



pCLIENT_LINK CreateClientLink(SOCKET ClientSock)
{
	pCLIENT_LINK Client = malloc(sizeof(CLIENT_LINK));
	Client->Buffer = malloc(BUF_LEN);
	Client->BufLenNow = BUF_LEN;
	Client->WSABuf.buf = Client->Buffer;
	Client->WSABuf.len = BUF_LEN;
	Client->ClientSock = ClientSock;

	return Client;
}

BOOL DestoryClientLink(pCLIENT_LINK Client)
{
	if (Client)
	{
		closesocket(Client->ClientSock);
		free(Client->Buffer);
		free(Client);
	}

	return TRUE;
}