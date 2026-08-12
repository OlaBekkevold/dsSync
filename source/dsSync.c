#include <nds.h>
#include <stdio.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include "connection.h"
#include "saveMenu.h"
#include <fat.h>
#include <dirent.h>


typedef struct
{
    char *buf;
    int pos;
    int cap;
} BufferSink;

void appendToBuffer(const char *data, int len, void *userData);
int getSaves();
int getLocalSaves();
int appendDynArray(char ***arr, int *capacity, int *size, const char *str);




int main(void)
{
    consoleDemoInit();

    if (isDSiMode())
    {
        iprintf("DSiMode enabled\n");
    } else
    {
        iprintf("DSiMode disabled\n");
    }

    iprintf("dsSync started\n");

    if (!fatInitDefault())
    {
        iprintf("Fat initialisation failed\n");
        return -1;
    }

    iprintf("Initializing Wifi...");
    if (!Wifi_InitDefault(WFC_CONNECT))
    {
        iprintf("Wifi connection failed\n");
        return -1;
    }
    iprintf("Wifi connection established\n");

    // if (conn_init("192.168.1.229", 3000) == -1) return -1;


    getLocalSaves();

    drawMenu();

    while (pmMainLoop())
    {
        int result = updateMenu();
        swiWaitForVBlank();
        int keys = keysDown();
        if (keys & KEY_START) break;
    }
    conn_close();
    return 0;
}

int appendDynArray(char ***arr, int *capacity, int *size, const char *str)
{

    if (*size >= *capacity)
    {
        *capacity *= 2;
        char **tmp = realloc(*arr, *capacity * sizeof(char *));
        if (tmp == NULL)
        {
            iprintf("Realloc failed\n");
            for (int i = 0; i < *size; i++)
            {
                free((*arr)[i]);
            }
            free(*arr);
            return -1;
        }
        *arr = tmp;
    }
    (*arr)[*size] = strdup(str);
    (*size)++;
    return 0;
}


int getLocalSaves()
{
    struct dirent *pent;
    DIR* pdir = opendir("/");

    if (!pdir)
    {
        iprintf("Error opening directory\n");
        closedir(pdir);
        return -1;
    }

    int capacity = 5;
    int size = 0;
    char **arr = calloc(capacity, sizeof(char *));
    if (arr == NULL)
    {
        iprintf("Realloc failed\n");
        return -1;
    }

    while ((pent = readdir(pdir)) != NULL)
    {
        appendDynArray(&arr, &capacity, &size, pent->d_name);
    }
    populateSaves((const char * const *)arr, size);

    for (int i = 0; i < size; i++)
    {
        free(arr[i]);
    }
    free(arr);
    closedir(pdir);
    return 0;
}


void appendToBuffer(const char *data, int len, void *userData)
{
    BufferSink *sink = (BufferSink *)userData;
    if (sink->pos + len < sink->cap)
    {
        memcpy(sink->buf + sink->pos, data, len);
        sink->pos += len;
        sink->buf[sink->pos] = 0;
    } else
    {
        iprintf("Buffer overflow\n");
    }

}

int getSaves()
{
    if (conn_ensure_ready() == -1) return -1;

    const char *requestText =
        "GET /saves HTTP/1.1\r\n"
        "Host: 192.168.1.229:3000\r\n"
        "User-Agent: DsSync\r\n\r\n";

    if (conn_send(requestText, strlen(requestText)) == -1) return -1;


    static char listBuf[2096];
    BufferSink sink = {listBuf, 0, sizeof(listBuf)};
    ResponseInfo info;
    conn_recv(&info, appendToBuffer, &sink);


    int capacity = 5;
    int size = 0;
    char **arr = calloc(capacity, sizeof(char *));
    if (arr == NULL)
    {
        iprintf("Realloc failed\n");
        return -1;
    }

    char *token = strtok(listBuf, "\r\n");
    while (token != NULL)
    {
        appendDynArray(&arr, &capacity, &size, token);

        token = strtok(nullptr, "\r\n");
    }

    populateSaves((const char * const *)arr, size);

    for (int i = 0; i < size; i++)
    {
        free(arr[i]);
    }
    free(arr);
    return 0;
}