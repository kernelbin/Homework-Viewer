#pragma once
#include<Windows.h>
#include<WinSock2.h>

HANDLE hCompPort;//IO������ɶ˿ھ��


#define SEND_FIN 1
#define RECV_FIN 2

typedef struct __IOCPModePack
{
	OVERLAPPED Overlapped;
	int SendRecvFin;
}IOCPMODEPACK, * pIOCPMODEPACK;

typedef struct __ClientLink
{
	WSABUF WSABuf;
	PBYTE Buffer;//������
	int BufLenNow;
	SOCKET ClientSock;
}CLIENT_LINK, * pCLIENT_LINK;

pIOCPMODEPACK  OverlappedWithSendFin, OverlappedWithRecvFin;


SOCKET PortSock;//�����˿���socket