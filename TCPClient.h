#ifndef __TCPCLIENT_H_
#define __TCPCLIENT_H_
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct {
  int fd;
  char* readBuffer; /*Allocated in TCPClient_Read, free'd in dispose*/
  char* writeBuffer; /*Allocated in TCPClient_Read, free'd in dispose*/
} TCPClient;


int TCPClient_Initiate(TCPClient* _Client, const char* _Host, const char* _Port);
int TCPClient_InitiatePtr(TCPClient** _ClientPtr, const char* _Host, const char* _Port);
int TCPClient_Read(TCPClient* _Client);
int TCPClient_Write(TCPClient* _Client, size_t _Length);
void TCPClient_Dispose(TCPClient* _Client);
void TCPClient_DisposePtr(TCPClient** _ClientPtr);


#endif /*__TCPCLIENT_H_*/
