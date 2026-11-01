#include "global.h"
#include "scene_input.h"
#include "scene_item_values.h"

struct StorageCategoryWidgets {
    u8 unk_00[0x318];
    struct SceneInputWidget carried[4][4];
    struct SceneInputWidget stored[4][4];
    u8 unk_F18[0x1E0];
    struct SceneInputWidget markers[8];
    u8 unk_13F8[0x140];
    struct SceneInputResource resource;
    u8 unk_1558[0x1FE];
    u8 emptyCarried;
    s8 emptyStored;
    u8 unk_1758[0x38];
    u8 slots[32];
};
s32 Storage_FindEquippedItemSlot(struct StorageCategoryWidgets *, s32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_GetSlotPosition(void *, s32);

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

#define STORAGE_MEMBER_OFFSET(member) ((u32) & ((struct StorageCategoryWidgets *)0)->member)

static inline void ClearOffset(u32 *offset)
{
    *offset = 0;
}
#define STORAGE_CLEAR_EMPTY(p)                                                                     \
    do {                                                                                           \
        (p)->emptyCarried = 255;                                                                   \
        (p)->emptyStored = -1;                                                                     \
    } while (0)
void Storage_RefreshEquipmentSlots(struct StorageCategoryWidgets *p)
{
    s32 col, row, marker, equipmentId;
    struct StorageCategoryWidgets *cellBase;
    u32 widgetOffset;
    u32 rowBytes;
    u32 rowSlot;
    u32 markerBytes;
    u32 markerStart;
    STORAGE_CLEAR_EMPTY(p);
    marker = 0;
    row = 0;
    rowBytes = 0;
    ClearOffset(&rowSlot);
    ClearOffset(&markerBytes);
    do {
        u32 *hideFlags;
        struct StorageCategoryWidgets *markerBase;
        u32 markerOffset;
        col = 0;
        {
            u32 *base = &p->carried[0][0].flags;
            hideFlags = (void *)(rowBytes + (u32)base);
        }
        cellBase = (void *)(rowBytes + (u32)p);
        widgetOffset = rowBytes;
        markerBase = (void *)(markerBytes + (u32)p);
        markerOffset = markerBytes;
        do {
            u32 index = rowSlot + col;
            equipmentId = Inventory_GetEquipmentId(p->slots[index]);
            if (equipmentId != 255) {
                struct SceneInputWidget *widget;
                ShowWidget(&cellBase->carried[0][0]);
                widget = (void *)((u8 *)&p->carried[0][0] + widgetOffset);
                Anim_LoadKeyframe(widget, &p->resource, (u16)(equipmentId + 240), 0);
                if (Storage_FindEquippedItemSlot(p, p->slots[index]) >= 0) {
                    struct SceneInputWidget *mark;
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                    markerStart = STORAGE_MEMBER_OFFSET(markers);
                    ShowWidget((void *)((u8 *)markerBase + markerStart));
                    mark = (void *)((u8 *)p + (markerOffset + markerStart));
                    Storage_GetSlotPosition(&mark->position, p->slots[index]);
                    markerBase = (void *)((u8 *)markerBase + sizeof(struct SceneInputWidget));
                    markerOffset += sizeof(struct SceneInputWidget);
                    markerBytes += sizeof(struct SceneInputWidget);
                    marker++;
                } else if (!Inventory_CanAssignEquipment(p->slots[index])) {
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                } else {
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
                }
            } else {
                if ((s8)p->emptyCarried < 0)
                    p->emptyCarried = index;
                *hideFlags |= 1;
            }
            hideFlags += sizeof(struct SceneInputWidget) / sizeof(*hideFlags);
            cellBase = (void *)((u8 *)cellBase + sizeof(struct SceneInputWidget));
            widgetOffset += sizeof(struct SceneInputWidget);
            col++;
            if (col > 3)
                break;
        } while (1);
        rowBytes += sizeof(p->carried[0]);
        rowSlot += 4;
        if (++row > 3)
            goto stored;
    } while (1);
stored:
    for (row = 0; row < 4;) {
        struct SceneInputWidget *widget;
        u32 rowSlot;
        u32 index;
        s32 nextRow;
        u32 offset;
        struct SceneInputWidget *base;
        col = 0;
        rowSlot = row * 4;
        nextRow = row + 1;
        offset = ((row << 3) + rowSlot) << 5;
        base = &p->stored[0][0];
        widget = (void *)(offset + (u32)base);
        index = rowSlot + 16;
    nextStored:
        equipmentId = Inventory_GetEquipmentId(p->slots[index]);
        if (equipmentId != 255) {
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, &p->resource, (u16)(equipmentId + 240), 1);
        } else {
            if (p->emptyStored < 0)
                p->emptyStored = index;
            widget->flags |= 1;
        }
        widget++;
        index++;
        col++;
        if (col <= 3)
            goto nextStored;
        row = nextRow;
    }
    for (; marker < 8; marker++)
        p->markers[marker].flags |= 1;
}
