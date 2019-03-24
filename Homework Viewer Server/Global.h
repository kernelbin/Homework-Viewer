#pragma once
#include<Windows.h>
#include<WinSock2.h>

HANDLE hCompPort;//IO操作完成端口句柄


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
	PBYTE Buffer;//缓冲区
	int BufLenNow;
	SOCKET ClientSock;
}CLIENT_LINK, * pCLIENT_LINK;

pIOCPMODEPACK  OverlappedWithSendFin, OverlappedWithRecvFin;


SOCKET PortSock;//监听端口用socket