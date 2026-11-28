#include "global.h"
#include "ui/widgets/scene_input.h"
#include "ui/town/shop_item_values.h"
#include "ui/town/storage_screen.h"

s32 Storage_FindEquippedItemSlot(struct StorageScreen *, s32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_GetSlotPosition(void *, s32);

#define STORAGE_MEMBER_OFFSET(member) ((u32) & ((struct StorageScreen *)0)->member)

static inline void Storage_ClearOffset(u32 *offset)
{
    *offset = 0;
}

void Storage_RefreshEquipmentSlots(struct StorageScreen *p)
{
    s32 col, row, marker;
    s32 id;
    struct StorageScreen *cellBase;
    u32 widgetOffset;
    u32 rowBytes;
    u32 rowSlot;
    u32 markerBytes;
    p->emptyCarried = 255;
    p->emptyStored = -1;
    marker = 0;
    row = 0;
    rowBytes = 0;
    Storage_ClearOffset(&rowSlot);
    Storage_ClearOffset(&markerBytes);
    do {
        u32 *hideFlags;
        struct StorageScreen *markerBase;
        u32 markerOffset;
        col = 0;
        {
            u32 *base = &p->firstSlots[0].flags;
            hideFlags = (void *)(rowBytes + (u32)base);
        }
        cellBase = (void *)(rowBytes + (u32)p);
        widgetOffset = rowBytes;
        markerBase = (void *)(markerBytes + (u32)p);
        markerOffset = markerBytes;
        do {
            s32 index = rowSlot + col;
            id = Inventory_GetEquipmentId(p->slots[index]);
            if (id != 255) {
                struct AnimSprite *animSprite;
                AnimSprite_Show(&cellBase->firstSlots[0]);
                {
                    u32 offset = widgetOffset + STORAGE_MEMBER_OFFSET(firstSlots);
                    animSprite = (void *)((u32)p + offset);
                }
                Anim_LoadKeyframe(animSprite, &p->storageResource, (u16)(id + 240), 0);
                if (Storage_FindEquippedItemSlot(p, p->slots[index]) >= 0) {
                    struct AnimSprite *mark;
                    SceneHud_SetKeyframePaletteBank(animSprite, 0x374);
                    AnimSprite_Show(&markerBase->categorySlots[0]);
                    {
                        u32 offset = STORAGE_MEMBER_OFFSET(categorySlots) + markerOffset;
                        mark = (void *)((u32)p + offset);
                    }
                    Storage_GetSlotPosition(&mark->position, p->slots[index]);
                    markerBase = (void *)((u8 *)markerBase + sizeof(struct AnimSprite));
                    markerOffset += sizeof(struct AnimSprite);
                    markerBytes += sizeof(struct AnimSprite);
                    marker++;
                } else if (!Inventory_CanAssignEquipment(p->slots[index])) {
                    SceneHud_SetKeyframePaletteBank(animSprite, 0x374);
                } else {
                    SceneHud_SetKeyframePaletteBank(animSprite, 0x373);
                }
            } else {
                if ((s8)p->emptyCarried < 0)
                    p->emptyCarried = index;
                *hideFlags |= 1;
            }
            hideFlags += sizeof(struct AnimSprite) / sizeof(*hideFlags);
            cellBase = (void *)((u8 *)cellBase + sizeof(struct AnimSprite));
            widgetOffset += sizeof(struct AnimSprite);
            col++;
            if (col > 3)
                goto carriedRowDone;
        } while (1);
    carriedRowDone:
        rowBytes += 4 * sizeof(struct AnimSprite);
        rowSlot += 4;
        row++;
        if (row > 3)
            goto carriedDone;
    } while (1);
carriedDone:
    for (row = 0; row < 4;) {
        struct AnimSprite *animSprite;
        s32 slot;
        s32 index;
        s32 nextRow;
        u32 offset;
        struct AnimSprite *base;
        col = 0;
        slot = row * 4;
        nextRow = row + 1;
        offset = ((row << 3) + slot) << 5;
        base = &p->secondSlots[0];
        animSprite = (void *)(offset + (u32)base);
        index = slot + 16;
        do {
            id = Inventory_GetEquipmentId(p->slots[index]);
            if (id != 255) {
                animSprite->flags &= ~1;
                Anim_LoadKeyframe(animSprite, &p->storageResource, (u16)(id + 240), 1);
            } else {
                if (p->emptyStored < 0)
                    p->emptyStored = index;
                animSprite->flags |= 1;
            }
            animSprite++;
            index++;
            col++;
            if (col > 3)
                goto storedRowDone;
        } while (1);
    storedRowDone:
        row = nextRow;
    }
    for (; marker < 8; marker++)
        p->categorySlots[marker].flags |= 1;
}
