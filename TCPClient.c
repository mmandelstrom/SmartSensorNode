#include "TCPClient.h"
#include <netdb.h>
#include <netinet/in.h>

int TCPClient_Initiate(TCPClient* _Client, const char* _Host, const char* _Port) {
  _Client->fd = -1;

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

  freeaddrinfo(addr_info);
  if (fd < 0)
    return -1;

  _Client->fd = fd;

  return 0; 
}



int TCPClient_InitiatePtr(TCPClient** _ClientPtr, const char* _Host, const char* _Port) {
  if (!_ClientPtr) {
    return -1;
  }
  TCPClient* client = (TCPClient*)malloc(sizeof(TCPClient));
  if (!client) {
    perror("malloc");
    return -2;
  }
  int result = TCPClient_Initiate(client, _Host, _Port);
  if (result != 0) {
    free(client);
    return -3;
  }
  
  *(_ClientPtr) = client;

  return 0;
}
void TCPClient_Dispose(TCPClient* _Client) {
  if (_Client == NULL) {
    return;
  }
  if (_Client->fd >= 0) {
    close(_Client->fd);
    _Client->fd = -1;
  }

  memset(_Client, 0, sizeof(TCPClient));
}

void TCPClient_DisposePtr(TCPClient** _ClientPtr) {
  if (_ClientPtr == NULL || *(_ClientPtr) == NULL) {
    return;
  }
  TCPClient_Dispose(*(_ClientPtr));
  free(*(_ClientPtr));
  *(_ClientPtr) = NULL;
}



