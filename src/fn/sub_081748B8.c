#define InventoryGrid_GetSlotPosition sub_081748B8
#include "global.h"
struct InventoryGridPosition {
    s16 x, y;
};
void sub_081748B8(struct InventoryGridPosition *p, s32 slot)
{
    if (slot <= 15) {
        p->x = 0x90 + (slot & 3) * 24;
        p->y = 0x20 + (slot >> 2) * 24;
    } else if (slot == 39) {
        p->x = 216;
        p->y = 80;
    } else if (slot == 36) {
        p->x = 216;
        p->y = 104;
    } else if (slot == 32) {
        p->x = 64;
        p->y = 76;
    } else if (slot == 33) {
        p->x = 88;
        p->y = 76;
    } else if (slot == 34) {
        p->x = 64;
        p->y = 104;
    } else if (slot == 35) {
        p->x = 88;
        p->y = 104;
    } else {
        p->x = 144;
        p->y = 32;
    }
}
