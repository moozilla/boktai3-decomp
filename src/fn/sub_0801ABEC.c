#include "global.h"
void SelectionMenu_ActorAvailability(void *);
void Bg_ClearTilemap(int);
void SelectionMenu_ConfigureLayers(int, int);
void SelectionMenu_PositionCursors(void *);
void SelectionMenu_PositionInventoryMarkers(void *);
void SelectionMenu_PopulateActors(void *);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_SetBounds(int, int, int, int);
void MessageWindow_SetDisplayMode(int);
void MessageWindow_SetPalette(int);
void SelectionMenu_OpenActorPage(u8 *p)
{
    SelectionMenu_ActorAvailability(p);
    p[0x1e] = 0;
    p[0x24] = 0;
    Bg_ClearTilemap(0);
    SelectionMenu_ConfigureLayers(13, 0);
    SelectionMenu_PositionCursors(p);
    SelectionMenu_PositionInventoryMarkers(p);
    SelectionMenu_PopulateActors(p);
    MessageWindow_SetMessageTable(*(u32 *)(p + 0xda8));
    MessageWindow_SetBounds(2, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x81b);
}
