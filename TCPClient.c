#include "TCPClient.h"

int TCPClient_Initiate(TCPClient* _Client, const char* _Host, const char* _Port, TCPClient_DataHandler _OnData, void* _Ctx) {
  _Client->fd = -1;
  _Client->readBuffer = NULL;
  _Client->writeBuffer = NULL;
  _Client->on_data = _OnData;
  _Client->on_data_ctx = _Ctx;

  struct addrinfo addresses = {0};
  struct addrinfo* result = NULL;

  addresses.ai_family = AF_UNSPEC; /*Allows both IPV4 & IPV6*/
  addresses.ai_socktype = SOCK_STREAM;
  addresses.ai_protocol = IPPROTO_TCP;

  int rc = getaddrinfo(_Host, _Port, &addresses, &result);
  if (rc != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
    return rc;
  }

  int fd = -1;
  struct addrinfo *addr_info;

  for (addr_info = result; addr_info; addr_info = addr_info->ai_next) {
    fd = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    
    if (fd < 0) continue;

    if (connect(fd, addr_info->ai_addr, addr_info->ai_addrlen) == 0) break;

    close(fd);
    fd = -1;

  }

  freeaddrinfo(result);
  if (fd < 0)
    return -1;

  _Client->fd = fd;

  return 0; 
}

int TCPClient_InitiatePtr(TCPClient** _ClientPtr, const char* _Host, const char* _Port, TCPClient_DataHandler _OnData, void* _Ctx) {
  if (!_ClientPtr) {
    return -1;
  }
  TCPClient* client = (TCPClient*)malloc(sizeof(TCPClient));
  if (!client) {
    perror("malloc");
    return -2;
  }
  int result = TCPClient_Initiate(client, _Host, _Port, _OnData, _Ctx);
  if (result != 0) {
    free(client);
    return -3;
  }
  
  *(_ClientPtr) = client;

  return 0;
}

int TCPClient_Read(TCPClient* _Client) {
  if (!_Client) {
    return -1;
  }

  if (_Client->fd < 0) {
    return -2;
  }

  if (_Client->readBuffer) {
    free(_Client->readBuffer);
    _Client->readBuffer = NULL;
  }

  size_t capacity = 512;
  _Client->readBuffer = (char*)malloc(capacity + 1);
  if (!_Client->readBuffer) {
    perror("malloc");
    return -3;
  }
  ssize_t bytesRead;
  size_t usedSpace = 0;
  size_t spaceLeft = capacity;

  while(1) {

    if (usedSpace >= capacity) {
      size_t newCapacity = capacity * 2;
      char* tempBuffer = (char*)realloc(_Client->readBuffer, newCapacity + 1);
      if (!tempBuffer) {
        free(_Client->readBuffer);
        _Client->readBuffer = NULL;
        perror("realloc");
        return -4;
      }
      capacity = newCapacity;
      _Client->readBuffer = tempBuffer;
      spaceLeft = capacity - usedSpace;
    }
    
    bytesRead = recv(_Client->fd, _Client->readBuffer + usedSpace, spaceLeft, 0);

    if (bytesRead < 0) {
      if (errno == EINTR) continue;
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        _Client->readBuffer[usedSpace] = '\0';
        if (_Client->on_data) {
          _Client->on_data(_Client->readBuffer, usedSpace, _Client->on_data_ctx);
        }
        return (int)usedSpace;
      }
      free(_Client->readBuffer);
      _Client->readBuffer = NULL;
      perror("recv");
      return -5;
    }
   

    if (bytesRead == 0) {
      _Client->readBuffer[usedSpace] = '\0';

      if (_Client->on_data) {
        _Client->on_data(_Client->readBuffer, usedSpace, _Client->on_data_ctx);
      }

      return (int)usedSpace;
    }

    if (bytesRead > 0) {
      usedSpace += (size_t)bytesRead;
      spaceLeft = capacity - usedSpace;
    }
  }
}


int TCPClient_Write(TCPClient* _Client, size_t _Length) {
  if (!_Client || _Client->fd < 0 || !_Client->writeBuffer) {
    return -1;
  }
  
  size_t bytesLeft = _Length;
  size_t totalSent = 0;
  ssize_t bytesSent;
  char* message = _Client->writeBuffer;

  while (bytesLeft > 0) {

      bytesSent = send(_Client->fd, message, bytesLeft, 0);

      if (bytesSent < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          return totalSent;
        }

        perror("send");
        return -2; // fatal error
      }

      if (bytesSent == 0) {
        return (int)totalSent;
      }

      totalSent += bytesSent;
      bytesLeft -= bytesSent;
      message += bytesSent;
  }
    return (int)totalSent;
}

void TCPClient_Dispose(TCPClient* _Client) {
  if (_Client == NULL) {
    return;
  }
  if (_Client->fd >= 0) {
    close(_Client->fd);
    _Client->fd = -1;
  }
  if (_Client->readBuffer != NULL) {
    free(_Client->readBuffer);
    _Client->readBuffer = NULL;
  }
  if (_Client->writeBuffer != NULL) {
    free(_Client->writeBuffer);
    _Client->writeBuffer = NULL;
  }
}

void TCPClient_DisposePtr(TCPClient** _ClientPtr) {
  if (_ClientPtr == NULL || *(_ClientPtr) == NULL) {
    return;
  }
  TCPClient_Dispose(*(_ClientPtr));
  free(*(_ClientPtr));
  *(_ClientPtr) = NULL;
}



