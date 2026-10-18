#define MapMenu_InitializePageBounds sub_0816BA90
#include "global.h"
struct MapMenuPage {
    u8 unknown0[2];
    u8 focusX, focusY, widthTiles, heightTiles;
    u8 unknown6[10];
};
struct MapMenu {
    u8 unknown0[0x48F8];
    struct MapMenuPage *pages;
    u8 unknown48FC[8];
    u8 page;
    u8 unknown4905[0xB];
    s16 minScrollX, minScrollY;
    s16 maxScrollX, maxScrollY, focusX, focusY;
};
void sub_0816BA90(struct MapMenu *menu)
{
    menu->minScrollX = 0;
    menu->minScrollY = 0;
    menu->maxScrollX = (menu->pages[menu->page].widthTiles - 30) * 8;
    menu->maxScrollY = (menu->pages[menu->page].heightTiles - 20) * 8;
    menu->focusX = menu->pages[menu->page].focusX * 8;
    menu->focusY = menu->pages[menu->page].focusY * 8;
}
