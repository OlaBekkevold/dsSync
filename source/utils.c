#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// DYNAMIC ARRAY OF STRINGS

DynArray initDynArray(const int capacity)
{
    char **arr = calloc(capacity, sizeof(char *));
    if (arr == NULL)
    {
        iprintf("Calloc failed\n");
        return (DynArray){.arr = NULL, .capacity = 0, .size = 0};
    }
    return (DynArray){arr, capacity, 0};
}

int appendDynArray(DynArray *arrStruct, const char *str)
{

    if (arrStruct->size >= arrStruct->capacity)
    {
        arrStruct->capacity *= 2;
        char **tmp = realloc(arrStruct->arr, arrStruct->capacity * sizeof(char *));
        if (tmp == NULL)
        {
            iprintf("Realloc failed\n");
            for (int i = 0; i < arrStruct->size; i++)
            {
                free((arrStruct->arr)[i]);
            }
            free(arrStruct->arr);
            return -1;
        }
        arrStruct->arr = tmp;
    }
    (arrStruct->arr)[arrStruct->size] = strdup(str);
    (arrStruct->size)++;
    return 0;
}

void freeDynArray(DynArray *arrStruct)
{
    for (int i = 0; i < arrStruct->size; i++)
    {
        free(arrStruct->arr[i]);
    }
    free(arrStruct->arr);
    // Nulling to prevent use-after-free and double-free bugs
    arrStruct->arr = NULL;
    arrStruct->capacity = 0;
    arrStruct->size = 0;
}
