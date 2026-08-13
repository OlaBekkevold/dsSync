#ifndef HTTPGET_UTILS_H
#define HTTPGET_UTILS_H

typedef struct
{
    char **arr;
    int capacity;
    int size;
} DynArray;

DynArray initDynArray(int capacity);
int appendDynArray(DynArray *arrStruct, const char *str);
void freeDynArray(DynArray *arrStruct);

#endif //HTTPGET_UTILS_H
