#ifndef HTTPGET_SAVEMENU_H
#define HTTPGET_SAVEMENU_H

#define MAX_SAVES 10

typedef struct
{
    char saveNames[MAX_SAVES][64];
    int saveCount;
    int currentIndex;
} SaveMenu;

void drawMenu();
int updateMenu();
int populateSaves(const char * const *saves, int count);

#endif //HTTPGET_SAVEMENU_H
