#ifndef HTTPGET_CONNECTION_H
#define HTTPGET_CONNECTION_H
#include <stddef.h>

typedef enum { CONN_DOWN, CONN_UP, CONN_RECONNECTING } conn_state_t;

typedef struct
{
    int sockfd;
    conn_state_t state;
    char server_ip[16];
    unsigned int server_port;
} connection_t;

extern connection_t g_conn;

typedef struct
{
    int statusCode;
    int contentLength;
} ResponseInfo;

typedef void (*BodyChunkHandler)(const char *data, int len, void *userData);


int conn_init(const char *ip, unsigned int port);
int conn_ensure_ready(void);
void conn_close(void);
int conn_send(const void *data, size_t len);
int conn_recv(ResponseInfo *info, BodyChunkHandler handler, void *userData);


#endif