#include "global.h"
struct StorageWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x54];
};
struct StorageItems {
    u8 unknown0[0x318];
    struct StorageWidget carried[16], stored[16];
    u8 unknownF18[0x1E0];
    struct StorageWidget markers[8];
    u8 unknown13F8[0x35E];
    u8 emptyCarried;
    s8 emptyStored;
    u8 unknown1758[0x38];
    u8 slots[32];
};
s32 Inventory_GetItemId(u32);
u32 Selection_GetDisplayFrame(s32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_RefreshItemSlots(struct StorageItems *p)
{
    s32 row;
    s32 col;
    u32 rowBytes;
    s32 rowSlot;
    p->emptyCarried = 0xFF;
    p->emptyStored = -1;
    row = 0;
    rowBytes = 0;
    for (; row < 4; row++) {
        struct StorageWidget *widget;
        col = 0;
        rowSlot = row * 4;
        {
            struct StorageWidget *base = p->carried;
            widget = (void *)(rowBytes + (u32)base);
        }
        do {
            s32 index = rowSlot + col;
            s32 id = Inventory_GetItemId(p->slots[index]);
            if (id >= 0) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, (u8 *)p + 0x1538, Selection_GetDisplayFrame(id), 1);
            } else {
                if ((s8)p->emptyCarried < 0)
                    p->emptyCarried = index;
                widget->flags |= 1;
            }
            SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            col++;
        } while (col <= 3);
        rowBytes += 0x180;
    }
    for (row = 0; row < 4;) {
        struct StorageWidget *widget;
        s32 index;
        s32 nextRow;
        col = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        {
            u32 offset = ((row << 3) + rowSlot) << 5;
            struct StorageWidget *base = p->stored;
            widget = (void *)(offset + (u32)base);
        }
        index = rowSlot + 16;
        do {
            s32 id = Inventory_GetItemId(p->slots[index]);
            if (id >= 0) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, (u8 *)p + 0x1538, Selection_GetDisplayFrame(id), 1);
            } else {
                if (p->emptyStored < 0)
                    p->emptyStored = index;
                widget->flags |= 1;
            }
            widget++;
            index++;
            col++;
            if (col > 3)
                goto storedRowDone;
        } while (1);
    storedRowDone:
        row = nextRow;
    }
    {
        u32 mask = 1;
        u32 *flags = &p->markers[0].flags;
        s32 i = 7;
        do {
            *flags |= mask;
            flags += 0x18;
        } while (--i >= 0);
    }
}
