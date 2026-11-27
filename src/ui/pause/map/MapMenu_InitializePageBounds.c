#include "global.h"
#include "ui/pause/map_menu.h"
struct MapMenuPage {
    u8 unknown0[2];
    u8 focusX, focusY, widthTiles, heightTiles;
    u8 unknown6[10];
};
void MapMenu_InitializePageBounds(struct MapMenu *menu)
{
    menu->minScrollX = 0;
    menu->minScrollY = 0;
    menu->maxScrollX = (menu->pages[menu->index].widthTiles - 30) * 8;
    menu->maxScrollY = (menu->pages[menu->index].heightTiles - 20) * 8;
    menu->focusX = menu->pages[menu->index].focusX * 8;
    menu->focusY = menu->pages[menu->index].focusY * 8;
}
