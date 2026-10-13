#include "global.h"
void SelectionMenu_ActorAvailability(void *);
void sub_082156B8(int);
void SelectionMenu_ConfigureLayers(int, int);
void SelectionMenu_PositionCursors(void *);
void SelectionMenu_PositionInventoryMarkers(void *);
void SelectionMenu_PopulateActors(void *);
void sub_08030C84(u32);
void sub_08030F20(int, int, int, int);
void sub_08030B78(int);
void sub_080311B0(int);
void SelectionMenu_OpenActorPage(u8 *p)
{
    SelectionMenu_ActorAvailability(p);
    p[0x1e] = 0;
    p[0x24] = 0;
    sub_082156B8(0);
    SelectionMenu_ConfigureLayers(13, 0);
    SelectionMenu_PositionCursors(p);
    SelectionMenu_PositionInventoryMarkers(p);
    SelectionMenu_PopulateActors(p);
    sub_08030C84(*(u32 *)(p + 0xda8));
    sub_08030F20(2, 16, 30, 2);
    sub_08030B78(1);
    sub_080311B0(0x81b);
}
