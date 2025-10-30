#include "TCPClient.h"
#include <stdio.h>

void PrintHandler(const char* data, size_t len, void* ctx) {
    (void)ctx; /*To avoid unused variable*/
    printf("\n[Callback] Received %zu bytes:\n%.*s\n", len, (int)len, data);
}

int main() {
  TCPClient* client;
  const char* host = "httpbin.org";
  const char* port = "80";
  const char* path = "/post";

  const char* json = "{\"message\":\"testingtesting\",\"value\":123}";
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

  if (TCPClient_InitiatePtr(&client, host, port, PrintHandler, NULL) < 0) {
    printf("Failed to initiate TCPClient\n");
    return -1;
  }
  
  size_t requestLength = strlen(httpRequest);
  client->writeBuffer = (char*)malloc(requestLength + 1);

  if (!client->writeBuffer) {
    perror("malloc");
    return -1;
  }

  strcpy(client->writeBuffer, httpRequest);
  printf("Sending request\n%s\n\n", client->writeBuffer);

  int sent = 0;
  while (sent < (int)requestLength) {
      int s = TCPClient_Write(client, requestLength);
      if (s < 0) {
          printf("TCPClient Write failed\n");
          return -1;
      }
      sent += s;
      usleep(5000); /*Backoff for nonblocking*/
  }

  int r = 0;
  while (r == 0) {
      r = TCPClient_Read(client);
      if (r < 0) {
          printf("TCPClient Read failed\n");
          return -1;
      }
      usleep(5000);
  }

   TCPClient_DisposePtr(&client);

  return 0;
}
