#include "connection.h"
#include <nds.h>
#include <stdio.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <string.h>

connection_t g_conn = { .sockfd =  -1, .state = CONN_DOWN };




int conn_ensure_ready(void)
{
    if (g_conn.state == CONN_UP) return 0;

    if (conn_init(g_conn.server_ip, g_conn.server_port) != 0)
    {
        iprintf("Unable to connect to server\n");
        return -1;
    }

    iprintf("Connection established\n");
    return 0;
}

int conn_init(const char* ip, unsigned int port)
{
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket == -1)
    {
        iprintf("Socket creation failed\n");
        return -1;
    }
    iprintf("socket created\n");

    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(port);
    sain.sin_addr.s_addr = inet_addr(ip);


    if (connect(mySocket, (struct sockaddr*)&sain, sizeof(sain)) == -1)
    {
        iprintf("Connection failed\n");
        return -1;
    }
    iprintf("Connected to server\n");
    g_conn.sockfd = mySocket;
    strncpy(g_conn.server_ip, ip, sizeof(g_conn.server_ip) - 1);
    g_conn.server_ip[sizeof(g_conn.server_ip) - 1] = '\0';
    g_conn.state = CONN_UP;
    return 0;
}

void conn_close(void)
{
    shutdown(g_conn.sockfd, 0);
    closesocket(g_conn.sockfd);
    g_conn.state = CONN_DOWN;
    iprintf("Closed connection\n");
}

int conn_send(const void* data, size_t len)
{
    if (conn_ensure_ready() == - 1) return -1;

    if (send(g_conn.sockfd, data, len, 0) == -1)
    {
        iprintf("Failed to send request\n");
        return -1;
    }
    iprintf("Sent %d bytes\n", len);
    return 0;
}

int conn_recv(ResponseInfo *info, BodyChunkHandler handler, void *userData)
{
    static char headerBuffer[1024];
    int headerLen = 0;
    int recvdLen;
    char *bodyStart = nullptr;

    while ((recvdLen = recv(g_conn.sockfd, headerBuffer + headerLen, sizeof(headerBuffer) - 1 - headerLen, 0)) > 0)
    {
        headerLen += recvdLen;
        headerBuffer[headerLen] = 0;

        bodyStart = strstr(headerBuffer, "\r\n\r\n");
        if (bodyStart)
        {
            bodyStart += 4;
            break;
        }
        if (headerLen >= (int)sizeof(headerBuffer) - 1)
        {
            iprintf("Header too large\n");
            return -1;
        }

    }
    if (!bodyStart)
    {
        iprintf("Failed to find end of header\n");
        return -1;
    }

    info->contentLength = 0;
    char *cl = strstr(headerBuffer, "Content-Length:");
    if (cl)
    {
        info->contentLength = atoi(cl + 16);
    } else
    {
        iprintf("Content-Length not found\n");
        return -1;
    }

    int alreadyRecvd = headerLen - (bodyStart - headerBuffer);
    if (alreadyRecvd > 0)
    {
        handler(bodyStart, alreadyRecvd, userData);
    }

    int bodyRecvd = alreadyRecvd;
    char chunk[512];
    int chunkLen = 0;
    while (bodyRecvd < info->contentLength)
    {
        chunkLen = recv(g_conn.sockfd, chunk, sizeof(chunk), 0);
        if (chunkLen <= 0) break;
        handler(chunk, chunkLen, userData);
        bodyRecvd += chunkLen;

    }

    return bodyRecvd == info->contentLength ? 0 : -1;


}
