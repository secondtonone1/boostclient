// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include <winsock2.h>  
#include <iostream>  

#include <string.h>  
#include <ctime>
#include<atltime.h>
using namespace std;  
#define HEADSIZE 4
#pragma comment(lib, "ws2_32.lib")          //add ws2_32.lib  
#include <vector>
int m_nMsgId = 0;
int m_nMsgLen = 0;
#define NUMMAX  1000
#define SLEEPTIME 500
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
DWORD WINAPI ThreadFunc(LPVOID p)
{   
	int     err,iLen; 
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
	//addrSrv.sin_addr.s_addr = inet_addr("192.168.3.133");
	addrSrv.sin_port = htons(DEFAULT_PORT);  

	//(5)connect  
	err = connect(sockClt,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));  

	if(err ==INVALID_SOCKET)  
	{  
		std::cout<<"connect() fail"<<WSAGetLastError()<<endl;  
		return -1;  
	}
	cout << "thread  "<<*(int*)(p) << " connect success "<<endl; 
	int i =1;
	int count = 0;
	Sleep( (NUMMAX-*(int*)(p))*SLEEPTIME );
	long long t1= (long long)time(0);
	while(count < 100)
	{
		char dataBuf[1024] ="123456789";
		char sendBuf[1024]={0};
		serializeHead(sendBuf,i,strlen(dataBuf));
		memcpy(sendBuf+HEADSIZE, dataBuf,strlen(dataBuf)+1);
		err = send(sockClt,sendBuf,strlen(dataBuf)+HEADSIZE,0);  
		//cout << "send data success : "<<dataBuf <<endl;
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
			//cout << "msgid : " << m_nMsgId <<endl;		
			//cout<<"datalength: "<< m_nMsgLen<<endl;		
			char dataBuff[1024]={0};
			memcpy(dataBuff, recvBuf+HEADSIZE, 1024) ;
			//cout <<"recv data is: "<< dataBuff <<endl;
			//i++;
			//if(i > 1700)
			//i=0;
			//	closesocket(sockClt);
			//break;
		
		}  
		count++;
	}
	long long t2= (long long)time(0);
	cout << "[thread  "<<*(int*)(p) << " cost seconds: "<<t2-t1<<"]"<<endl; 	
	closesocket(sockClt);  
	return 0;
}

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
	std::vector<HANDLE> vecHandle;
	vector<int> vecNum;
	for(int i=1; i<= NUMMAX; i++)
	{
		vecNum.push_back(i);
	}
	for(int i = 0; i < NUMMAX; i++)
	{
		HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, &vecNum[i], 0, 0); // 创建线程
		vecHandle.push_back(hThread);
		Sleep(SLEEPTIME);
	}  
	getchar();
	for(int i = 0; i < vecHandle.size(); i++)
	{
		CloseHandle(vecHandle[i]);
	}
	WSACleanup(); 
	return 0;  
}  


