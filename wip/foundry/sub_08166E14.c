#include "global.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
#define InventoryPanel_SetEntryRow sub_08166E14
struct InventoryPanelElement {
    u32 flags;
    u8 unknown4[0x14];
    s16 x, y;
    u8 unknown1C[0x44];
};
struct InventoryPanel {
    u8 unknown0[0x1428];
    struct InventoryPanelElement elements[1];
};
void sub_081654DC(s32, s32, s32, s32);
void sub_08166D04(struct InventoryPanel *, s32, s32, s32, s32);
void sub_08166E14(struct InventoryPanel *panel, s32 x, s32 y, s32 item, s32 kind, s32 row)
{
    struct InventoryPanelElement *e;
    s32 delta;
    if (item < 0) {
        u16 mask = 1;
        s32 i;
        delta = row * 0x60;
        e = panel->elements;
        e = (struct InventoryPanelElement *)(delta + (u32)e);
        i = 2;
        do {
            e->flags |= mask;
            e++;
        } while (--i >= 0);
        sub_081654DC(3, x, y, 0);
    } else {
        s32 yy;
        s32 xx;
        struct InventoryPanel *shifted;
        u32 mask;
        s32 offset;
        s32 i;
        sub_081654DC(3, x, y, -1);
        yy = y * 8;
        delta = row * 0x60;
        e = panel->elements;
        e = (struct InventoryPanelElement *)(delta + (u32)e);
        xx = x * 8;
        shifted = (struct InventoryPanel *)(row * 0x60 + (u32)panel);
        offset = 0x1442;
        mask = ~1;
        i = 2;
        do {
            e->x = xx;
            *(s16 *)((u8 *)shifted + offset) = yy;
            e->flags &= mask;
            e++;
            xx += 8;
            shifted = (struct InventoryPanel *)((u8 *)shifted + 0x60);
        } while (--i >= 0);
        sub_08166D04(panel, item, 100, kind, row);
    }
}
