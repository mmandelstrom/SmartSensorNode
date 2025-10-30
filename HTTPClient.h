#ifndef __HTTPCLIENT_H_
#define __HTTPCLIENT_H_

typedef struct
{

} HTTPClient;


int HTTPClient_Initiate(HTTPClient* _Client);
int HTTPClient_InitiatePtr(HTTPClient** _ClientPtr);
void HTTPClient_Dispose(HTTPClient* _Client);
void HTTPClient_DisposePtr(HTTPClient** _ClientPtr);


#endif /*__HTTPCLIENT_H_ */
