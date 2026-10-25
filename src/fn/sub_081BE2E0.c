#include "global.h"
#include "scene_input_navigation.h"

struct StorageWidget {
    u8 unk_00[8];
    u32 flags;
    u8 unk_0C[0x14];
    struct SceneInputPosition position;
    u8 unk_28[0x38];
};
struct StorageCategoryWidgets {
    u8 unk_0000[0x318];
    struct StorageWidget carried[16];
    struct StorageWidget stored[16];
    u8 unk_0F18[0x1E0];
    struct StorageWidget markers[8];
    u8 unk_13F8[0x140];
    u8 resource[0x21E];
    u8 emptyCarried;
    s8 emptyStored;
    u8 unk_1758[0x38];
    u8 slots[32];
};
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);

static inline void Storage_AdvanceSlotRow(u32 *slot)
{
    *slot += 4;
}

static inline void Storage_ClearOffset(u32 *offset)
{
    *offset = 0;
}

#ifdef NONMATCHING
// fuzzy: 90.1%
void Storage_RefreshConsumableSlots(struct StorageCategoryWidgets *p)
{
    s32 col, row;
    s32 equipped;
    struct StorageCategoryWidgets *entry;
    u32 rowBytes, rowSlot, markerBytes;
    p->emptyCarried = 0xFF;
    p->emptyStored = -1;
    equipped = 0;
    row = 0;
    rowBytes = 0;
    Storage_ClearOffset(&rowSlot);
    Storage_ClearOffset(&markerBytes);
    do {
        struct StorageWidget *widget;
        struct StorageCategoryWidgets *markerBase;
        u32 *flags;
        u32 markerOffset;
        col = 0;
        {
            u32 *base = &p->carried[0].flags;
            flags = (void *)(rowBytes + (u32)base);
        }
        entry = (void *)(rowBytes + (u32)p);
        markerBase = (void *)(markerBytes + (u32)p);
        markerOffset = markerBytes;
        widget = (void *)(rowBytes + (u32)p->carried);
        do {
            s32 index = rowSlot + col;
            s32 id = Inventory_GetConsumableId(p->slots[index]);
            if (id != -1) {
                struct StorageWidget *shown = entry->carried;
                shown->flags &= ~1;
                Anim_LoadKeyframe(widget, p->resource, (u16)(id + 0x121), 0);
                if (GameState_IsConsumableSlotEquipped(p->slots[index])) {
                    struct StorageWidget *marker;
                    u32 offset;
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                    offset = (u32) & ((struct StorageCategoryWidgets *)0)->markers;
                    marker = (void *)((u32)markerBase + offset);
                    marker->flags &= ~1;
                    offset += markerOffset;
                    {
                        u32 address = (u32)p + offset;
                        marker = (void *)address;
                    }
                    Storage_GetSlotPosition(&marker->position, p->slots[index]);
                    markerBase = (void *)((u32)markerBase + sizeof(struct StorageWidget));
                    markerOffset += sizeof(struct StorageWidget);
                    markerBytes += 0x60;
                    equipped++;
                } else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else {
                if ((s8)p->emptyCarried < 0)
                    p->emptyCarried = index;
                *flags |= 1;
            }
            flags += sizeof(struct StorageWidget) / sizeof(u32);
            entry = (void *)((u32)entry + sizeof(struct StorageWidget));
            widget++;
            col++;
            if (col > 3)
                break;
        } while (1);
        rowBytes += 0x180;
        /* Advance by the four carried slots processed in this row. */
        Storage_AdvanceSlotRow(&rowSlot);
        row++;
        if (row > 3)
            break;
    } while (1);
    for (row = 0; row < 4;) {
        struct StorageWidget *widget;
        s32 rowSlot;
        s32 index;
        s32 nextRow;
        u32 offset;
        col = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        offset = ((row << 3) + rowSlot) << 5;
        {
            struct StorageWidget *base = p->stored;
            widget = (void *)(offset + (u32)base);
        }
        index = rowSlot + 16;
        do {
            s32 id = Inventory_GetConsumableId(p->slots[index]);
            if (id != -1) {
                widget->flags &= ~1;
                Anim_LoadKeyframe(widget, p->resource, (u16)(id + 0x121), 0);
            } else {
                if (p->emptyStored < 0)
                    p->emptyStored = index;
                widget->flags |= 1;
            }
            widget++;
            index++;
            col++;
            if (col > 3)
                break;
        } while (1);
        row = nextRow;
    }
    while (equipped <= 7) {
        p->markers[equipped].flags |= 1;
        equipped++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Storage_RefreshConsumableSlots);
#endif
