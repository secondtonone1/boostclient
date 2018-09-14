// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include <winsock2.h>  
#include <iostream>  

#include <string.h>  
using namespace std;  
#define HEADSIZE 4
#pragma comment(lib, "ws2_32.lib")          //add ws2_32.lib  
int m_nMsgId = 0;
int m_nMsgLen = 0;
bool unserializeHead(char * msgHead)
{
	
	m_nMsgId = 0;
	m_nMsgLen = 0;
	//前两个字节按位存id
	//后两个字节按位存len
	//0x 0000 0001  0001 0001 = 273 小端存储

	for(int i = 0; i <2; i++)
	{
		for(int j =0; j < 8; j++)
		{
			if(msgHead[i]& (0x01<<j))
				m_nMsgId+=(0x01<<j);
		}
		m_nMsgId = m_nMsgId <<8*(1-i);
	}

	for(int i = 2; i <HEADSIZE; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(msgHead[i]&(0x01<<j))
				m_nMsgLen+= (0x01<<j);
		}
		m_nMsgLen=m_nMsgLen <<8*(HEADSIZE-i-1);
	}
	return true;
}

bool serializeHead(char * pData, unsigned short nMsgId, unsigned short nMsgLen)
{
	//前两个字节按位存id
	//后两个字节按位存len
	//0x 0000 0001  0001 0001 = 273
	for(unsigned int i =  0; i < 2; i++)
	{
		unsigned short nByte = (nMsgId >> 8*(1-i));
		for(int j = 0; j < 8; j++)
		{
			if(nByte & (0x01 <<j) )
				pData[i]=(pData[i]|(0x01 <<j));
		}
	}

	for(unsigned int i=2; i <HEADSIZE; i++)
	{
		unsigned short nByte = (nMsgLen >> 8*(HEADSIZE-i-1));
		for(int j=0; j<8;j++)
		{
			if(nByte & (0x01 <<j))
				pData[i]=(pData[i]|(0x01 <<j));		
		}
	}
	return true;
}

const int DEFAULT_PORT = 8898;  
int main(int argc,char* argv[])  
{  

	WORD    wVersionRequested;  
	WSADATA wsaData;  
	int     err,iLen;  
	wVersionRequested   =   MAKEWORD(2,2);//create 16bit data  
	//(1)Load WinSock  
	err =   WSAStartup(wVersionRequested,&wsaData); //load win socket  
	if(err!=0)  
	{  
		cout<<"Load WinSock Failed!";  
		return -1;  
	}  
	//(2)create socket  
	SOCKET sockClt = socket(AF_INET,SOCK_STREAM,0);  
	if(sockClt == INVALID_SOCKET){  
		cout<<"socket() fail:"<<WSAGetLastError()<<endl;  
		return -2;  
	}  
	//(3)IP  
	SOCKADDR_IN addrSrv;  
	addrSrv.sin_family = AF_INET;  
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	//addrSrv.sin_addr.s_addr = inet_addr("192.168.3.132");
	addrSrv.sin_port = htons(DEFAULT_PORT);  

	//(5)connect  
	err = connect(sockClt,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));  

	if(err ==INVALID_SOCKET)  
	{  
		cout<<"connect() fail"<<WSAGetLastError()<<endl;  
		return -1;  
	}  
	int i =1;
	while(1)
	{
		char dataBuf[1024] ="123456789";
		char sendBuf[1024]={0};
		serializeHead(sendBuf,i,strlen(dataBuf));
		memcpy(sendBuf+HEADSIZE, dataBuf,strlen(dataBuf)+1);
		err = send(sockClt,sendBuf,strlen(dataBuf)+sizeof(int)*2,0);  
		cout << "send data success : "<<dataBuf <<endl;
		char recvBuf[1024]="\0";  
		iLen = recv(sockClt,recvBuf,1024,0);  
		if(iLen ==0)  
			break;  
		else if(iLen==SOCKET_ERROR){  
			cout<<"recv() fail:"<<WSAGetLastError()<<endl;  
			break ;  
		}  
		else  
		{  
			unserializeHead(recvBuf);
			cout << "msgid : " << m_nMsgId <<endl;		
			cout<<"datalength: "<< m_nMsgLen<<endl;		
			char dataBuff[1024]={0};
			memcpy(dataBuff, recvBuf+HEADSIZE, 1024) ;
			cout <<"recv data is: "<< dataBuff <<endl;
			//i++;
			//if(i > 1700)
			//i=0;
		//	closesocket(sockClt);
			//break;
		}  
	}
	closesocket(sockClt);  

	WSACleanup();  
	system("PAUSE");  
	getchar();
	return 0;  
}  


