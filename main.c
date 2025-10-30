#include "TCPClient.h"
#include <stdio.h>

int main() {
  TCPClient client;
  const char* host = "httpbin.org";
  const char* port = "80";
  const char* path = "/post";

const char* json = "{\"message\":\"hej\",\"value\":123}";
char httpRequest[512];

if (snprintf(httpRequest, sizeof(httpRequest),
    "POST %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "\r\n"
    "%s",
    path, host, strlen(json), json) < 0) {
    return -1;
  }

  int result = TCPClient_Initiate(&client, host, port);
  
  size_t requestLength = strlen(httpRequest);
  client.writeBuffer = (char*)malloc(requestLength + 1);
  if (!client.writeBuffer) {
    perror("malloc");
    return -1;
  }

  strcpy(client.writeBuffer, httpRequest);

  printf("WB: %s\n", client.writeBuffer);

  int writeE = TCPClient_Write(&client, requestLength);
  if (writeE != 0) {
    printf("Write: %d\n", writeE);
  }

  int readD = TCPClient_Read(&client);
  printf("Read: %d\n", readD);
  if (client.readBuffer) {
    printf("Data receieved: %s\n", client.readBuffer);
  }



  if (result != 0) {
    printf("Failed to init tcp client\n");
    return -1;
  }

  printf("Init successfull\n");
  return 0;
}
