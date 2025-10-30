#include "TCPClient.h"
#include <stdio.h>

int main() {
  TCPClient client;
  const char* host = "portquiz.net";
  const char* port = "8080";
  int result = TCPClient_Initiate(&client, host, port);

  if (result != 0) {
    printf("Failed to init tcp client\n");
    return -1;
  }

  printf("Init successfull\n");
  return 0;
}
