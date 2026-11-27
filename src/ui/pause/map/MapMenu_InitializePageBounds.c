#include "global.h"
#include "ui/pause/scene_hud_display.h"
void MapMenu_InitializePageBounds(struct SceneHudState *menu)
{
    menu->mapMinScrollX = 0;
    menu->mapMinScrollY = 0;
    menu->mapMaxScrollX = (menu->navigationPages[menu->navigationPage].widthTiles - 30) * 8;
    menu->mapMaxScrollY = (menu->navigationPages[menu->navigationPage].heightTiles - 20) * 8;
    menu->mapFocusX = menu->navigationPages[menu->navigationPage].focusX * 8;
    menu->mapFocusY = menu->navigationPages[menu->navigationPage].focusY * 8;
}
