#include "saveMenu.h"
#include "nds.h"
#include "stdio.h"

static SaveMenu menu;

int populateSaves(const char * const *saves, int count)
{
    menu.saveCount = count;

    if (menu.saveCount > MAX_SAVES)
    {
        menu.saveCount = MAX_SAVES;
    }

    for (int i = 0; i < menu.saveCount; i++)
    {
        strcpy(menu.saveNames[i], saves[i]);
    }

    return 0;
}

void drawMenu()
{
    consoleClear();
    for (int i = 0; i < menu.saveCount; i++)
    {
        if (i == menu.currentIndex)
        {
            iprintf("> %d. %s\n", i+1, menu.saveNames[i]);
        } else
        {
            iprintf("  %d. %s\n", i+1, menu.saveNames[i]);
        }

    }
}

int updateMenu()
{
    scanKeys();
    int kDown = keysDown();

    if (kDown & KEY_UP)
    {
        menu.currentIndex--;
        if (menu.currentIndex < 0)
        {
            menu.currentIndex = menu.saveCount - 1;
        }
        drawMenu();
    }

    if (kDown & KEY_DOWN)
    {
        menu.currentIndex++;
        if (menu.currentIndex >= menu.saveCount)
        {
            menu.currentIndex = 0;
        }
        drawMenu();
    }

    return -2;
}
