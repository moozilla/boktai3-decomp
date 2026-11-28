#include "global.h"
#include "engine/video/anim_sprite.h"
#include "ui/town/storage_screen.h"
s32 Inventory_GetItemId(u32);
u32 Selection_GetDisplayFrame(s32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_RefreshItemSlots(struct StorageScreen *screen)
{
    s32 row;
    s32 col;
    u32 rowBytes;
    s32 rowSlot;
    screen->emptyCarried = 0xFF;
    screen->emptyStored = -1;
    row = 0;
    rowBytes = 0;
    for (; row < 4; row++) {
        struct AnimSprite *widget;
        col = 0;
        rowSlot = row * 4;
        {
            struct AnimSprite *base = screen->firstSlots;
            widget = (void *)(rowBytes + (u32)base);
        }
        do {
            s32 index = rowSlot + col;
            s32 id = Inventory_GetItemId(screen->slots[index]);
            if (id >= 0) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, &screen->storageResource,
                                  (u16)Selection_GetDisplayFrame(id), 1);
            } else {
                if ((s8)screen->emptyCarried < 0)
                    screen->emptyCarried = index;
                widget->flags |= 1;
            }
            SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            col++;
        } while (col <= 3);
        rowBytes += 0x180;
    }
    for (row = 0; row < 4;) {
        struct AnimSprite *widget;
        s32 index;
        s32 nextRow;
        col = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        {
            u32 offset = ((row << 3) + rowSlot) << 5;
            struct AnimSprite *base = screen->secondSlots;
            widget = (void *)(offset + (u32)base);
        }
        index = rowSlot + 16;
        do {
            s32 id = Inventory_GetItemId(screen->slots[index]);
            if (id >= 0) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, &screen->storageResource,
                                  (u16)Selection_GetDisplayFrame(id), 1);
            } else {
                if (screen->emptyStored < 0)
                    screen->emptyStored = index;
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
        s32 i;
        for (i = 0; i < 8; i++)
            screen->categorySlots[i].flags |= 1;
    }
}
