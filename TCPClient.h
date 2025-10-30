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

typedef struct {
  int fd;
} TCPClient;


int TCPClient_Initiate(TCPClient* _Client, const char* _Host, const char* _Port);
int TCPClient_InitiatePtr(TCPClient** _ClientPtr, const char* _Host, const char* _Port);
void TCPClient_Dispose(TCPClient* _Client);
void TCPClient_DisposePtr(TCPClient** _ClientPtr);


#endif /*__TCPCLIENT_H_*/
