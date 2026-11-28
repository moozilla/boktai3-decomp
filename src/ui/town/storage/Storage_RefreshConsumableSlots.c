#include "global.h"
#include "ui/widgets/scene_input_navigation.h"
#include "ui/town/storage_screen.h"

s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);

static inline void Storage_ClearOffset(u32 *offset)
{
    *offset = 0;
}

void Storage_RefreshConsumableSlots(struct StorageScreen *p)
{
    s32 col, row;
    s32 equipped;
    struct StorageScreen *entry;
    u32 rowBytes, rowSlot, markerBytes;
    p->emptyCarried = 0xFF;
    p->emptyStored = -1;
    equipped = 0;
    row = 0;
    rowBytes = 0;
    Storage_ClearOffset(&rowSlot);
    Storage_ClearOffset(&markerBytes);
    do {
        struct AnimSprite *widget;
        struct StorageScreen *markerBase;
        u32 *flags;
        u32 markerOffset;
        col = 0;
        {
            u32 *base = &p->firstSlots[0].flags;
            flags = (void *)(rowBytes + (u32)base);
        }
        entry = (void *)(rowBytes + (u32)p);
        markerBase = (void *)(markerBytes + (u32)p);
        markerOffset = markerBytes;
        widget = (void *)(rowBytes + (u32)p->firstSlots);
        do {
            s32 index = rowSlot + col;
            s32 id = Inventory_GetConsumableId(p->slots[index]);
            if (id != -1) {
                struct AnimSprite *shown = entry->firstSlots;
                shown->flags &= ~1;
                Anim_LoadKeyframe(widget, &p->storageResource, (u16)(id + 0x121), 0);
                if (GameState_IsConsumableSlotEquipped(p->slots[index])) {
                    struct AnimSprite *marker;
                    u32 offset;
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                    offset = (u32) & ((struct StorageScreen *)0)->categorySlots;
                    marker = (void *)((u32)markerBase + offset);
                    marker->flags &= ~1;
                    offset += markerOffset;
                    {
                        u32 address = (u32)p + offset;
                        marker = (void *)address;
                    }
                    Storage_GetSlotPosition(&marker->position, p->slots[index]);
                    markerBase = (void *)((u32)markerBase + sizeof(struct AnimSprite));
                    markerOffset += sizeof(struct AnimSprite);
                    markerBytes += 0x60;
                    equipped++;
                } else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else {
                if ((s8)p->emptyCarried < 0)
                    p->emptyCarried = index;
                *flags |= 1;
            }
            flags += sizeof(struct AnimSprite) / sizeof(u32);
            entry = (void *)((u32)entry + sizeof(struct AnimSprite));
            widget++;
            col++;
            if (col > 3)
                goto carriedRowDone;
        } while (1);
    carriedRowDone:
        rowBytes += 0x180;
        rowSlot += 4;
        row++;
        if (row > 3)
            goto carriedDone;
    } while (1);
carriedDone:
    for (row = 0; row < 4;) {
        struct AnimSprite *widget;
        s32 slot;
        s32 index;
        s32 nextRow;
        u32 offset;
        col = 0;
        slot = row * 4;
        nextRow = row + 1;
        offset = ((row << 3) + slot) << 5;
        {
            struct AnimSprite *base = p->secondSlots;
            widget = (void *)(offset + (u32)base);
        }
        index = slot + 16;
        do {
            s32 id = Inventory_GetConsumableId(p->slots[index]);
            if (id != -1) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, &p->storageResource, (u16)(id + 0x121), 0);
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
    while (equipped <= 7) {
        p->categorySlots[equipped].flags |= 1;
        equipped++;
    }
}
