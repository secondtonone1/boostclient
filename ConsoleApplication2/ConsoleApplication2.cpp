// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include <winsock2.h>  
#include <iostream>  

#include <string.h>  
using namespace std;  

#pragma comment(lib, "ws2_32.lib")          //add ws2_32.lib  


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
		int length = htonl(strlen(dataBuf));
		int m = htonl(i);
		memcpy(sendBuf, &m,sizeof(int));
		memcpy(sendBuf+sizeof(int), &length,sizeof(int));
		memcpy(sendBuf+sizeof(int)*2, dataBuf,strlen(dataBuf)+1);
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
			int nMsgId = 0;
			memcpy(&nMsgId, recvBuf,4);
			nMsgId = ntohl(nMsgId);
			cout << "msgid : " << nMsgId <<endl;
			int nLength = 0;
			memcpy(&nLength, recvBuf+4,4);
			nLength = ntohl(nLength);
			cout<<"datalength: "<< nLength<<endl;
			
			char dataBuff[1024]={0};
			memcpy(dataBuff, recvBuf+8, 1024) ;
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


