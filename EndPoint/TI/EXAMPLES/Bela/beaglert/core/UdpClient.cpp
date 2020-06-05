/*
 * udpClient.cpp
 *
 *  Created on: 19 May 2015
 *      Author: giulio moro
 */
#include "UdpClient.h"

	UdpClient::UdpClient(){
		outSocket=socket(AF_INET, SOCK_DGRAM, 0);
		isSetPort=false;
		isSetServer=false;
		enabled=false;
	}
	UdpClient::UdpClient(int aPort, const char* aServerName){
		outSocket=socket(AF_INET, SOCK_DGRAM, 0);
		if(outSocket<0){
			enabled=false;
			return;
		}
		setPort(aPort);
		setServer(aServerName);
		isSetPort=true;
		isSetServer=true;
		enabled=true;
    memset(&stTimeOut, 0, sizeof(struct timeval));
	}
	UdpClient::~UdpClient(){
	   close(outSocket);
	}
	void UdpClient::setPort(int aPort){
		port=aPort;
		destinationServer.sin_port = htons(port);
		destinationServer.sin_family = AF_INET;
		isSetPort=true;
		if(isSetServer){
			enabled=true;
		}
	};
	void UdpClient::setServer(const char* aServerName){
		inet_pton(AF_INET,aServerName,&destinationServer.sin_addr);
		isSetServer=true;
		if(isSetPort){
			enabled=true;
		}
	};
	int UdpClient::send(void * message, int size){
		if(!enabled)
			return -1;
		unsigned int length;
		length=sizeof(struct sockaddr_in);
		int n=sendto(outSocket,message,size,0,(const struct sockaddr *)&destinationServer,length);
		if (n < 0){
			return n;
		}
		return 1;
	};
	int UdpClient::write(const char* remoteHostname, int remotePortNumber, void* sourceBuffer, int numBytesToWrite){
		setServer(remoteHostname);
		setPort(remotePortNumber);
		send(sourceBuffer, numBytesToWrite);
	}
  int UdpClient::waitUntilReady(bool readyForReading, int timeoutMsecs){
//	If the socket is ready on return, this returns 1. If it times-out before the socket becomes ready, it returns 0. If an error occurs, it returns -1.
    if(enabled==false)
		return -1;
    if(timeoutMsecs<0)
		return select(outSocket+1, NULL, &stWriteFDS, NULL, NULL); //calling this with a NULL timeout will block indefinitely
    FD_ZERO(&stWriteFDS);
    FD_SET(outSocket, &stWriteFDS);
	float timeOutSecs=timeoutMsecs*0.001;
	stTimeOut.tv_sec=(int)timeOutSecs;
	timeOutSecs-=(int)timeOutSecs;
	stTimeOut.tv_usec=(int)(timeOutSecs*1000000);
    int descriptorReady= select(outSocket+1, NULL, &stWriteFDS, NULL, &stTimeOut);
    return descriptorReady>0? 1 : descriptorReady;
  }