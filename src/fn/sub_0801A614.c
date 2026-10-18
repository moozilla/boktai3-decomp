#include "global.h"
/* SelectionMenu_HideMarkers: Independent eight-marker hide loop in the byte tail, matching the eight inventory/equipment markers positioned by SelectionMenu_PositionInventoryMarke */
#define SelectionMenu_HideMarkers sub_0801A614
#define SelectionMenu_HideMarkers sub_0801A614
typedef struct {
    u32 flags;
    u8 pad[92];
} Graphic;
void SelectionMenu_HideMarkers(u8 *p)
{
    int mask = 1;
    Graphic *g;
    int i;
    g = (Graphic *)(p + 0x2bc);
    i = 7;
    do {
        g->flags |= mask;
        g++;
        i--;
    } while (i >= 0);
}
