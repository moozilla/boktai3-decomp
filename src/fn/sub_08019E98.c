#include "global.h"
u8 *Inventory_GetEquipmentEntry(u32);
void Presentation_SetActorIconFrame(u8 *, u8 *, u8 *);
void SelectionMenu_SetActorIcon(u8 *p, u8 *q, u32 c)
{
    u8 *r = Inventory_GetEquipmentEntry(c);
    if (*r != 0xff) {
        Presentation_SetActorIconFrame(p, q, r);
        *(u32 *)(q + 8) &= ~1;
    } else {
        *(u32 *)(q + 8) |= 1;
    }
}
