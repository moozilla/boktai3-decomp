#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "game_state.h"

struct EquipmentDescriptor {
    u8 unk_00[2];
    u8 requiredLevel;
    u8 unk_03[3];
    u16 group;
    u8 unk_08[24];
};
extern const struct EquipmentDescriptor gUnk_08E6092C[];
s32 Inventory_GetEquipmentId(u32);
void PauseMenu_SetSpritePalette(struct SceneInputWidget *, u16);

void InventoryGrid_GetSlotPosition(void *, s32);
s32 sub_0822D730(s32);

/* Tentative: refresh equipment icons, with a fixed icon in alternate action mode. */
// scope shape 151 (differences once low registers are set aside)
void SceneHud_RefreshEquipmentAvailability(struct SceneHudState *owner)
{
    s32 i;
    u32 *flags;
    u32 mask;

    if (owner->actionOwner->actionMode == 6) {
        struct SceneInputWidget *widget;
        owner->categoryWidgets[10].flags &= ~1;
        widget = &owner->categoryWidgets[10];
        Anim_LoadKeyframe(widget, owner->motionResource, 0x2F, 1);
        PauseMenu_SetSpritePalette(widget, 0x373);
        mask = 1;
        flags = &owner->categoryWidgets[11].flags;
        i = 14;
        do {
            *flags |= mask;
            flags += sizeof(*widget) / sizeof(*flags);
        } while (--i >= 0);
    } else {
        struct SceneInputWidget *widget;
        i = 0;
        widget = &owner->categoryWidgets[10];
        do {
            s32 id = Inventory_GetEquipmentId(i);
            if (id == 255) {
                owner->categoryWidgets[i + 10].flags |= 1;
            } else {
                owner->categoryWidgets[i + 10].flags &= ~1;
                Anim_LoadKeyframe(widget, owner->motionResource, (u16)id, 1);
                PauseMenu_SetSpritePalette(widget, 0x373);
                if (gGameState->level >= gUnk_08E6092C[id].requiredLevel)
                    PauseMenu_SetSpritePalette(widget, 0x373);
                else
                    PauseMenu_SetSpritePalette(widget, 0x374);
            }
            widget++;
            i++;
        } while (i <= 15);
    }
}
/* Tentative: configure one icon transfer between equipment-grid positions. */
void SceneHud_ConfigureEquipmentTransfer(struct SceneHudState *owner, s32 index, s32 from, s32 to)
{
    struct SceneInputWidget *widget = &owner->motionWidgets[index];
    void *motion;
    widget->flags &= ~1;
    motion = owner->motionResource;
    Anim_LoadKeyframe(widget, motion, (u16)Inventory_GetEquipmentId(from), 1);
    InventoryGrid_GetSlotPosition(&widget->position, from);
    InventoryGrid_GetSlotPosition(&owner->actionTargets[index], to);
    PauseMenu_SetSpritePalette(widget, owner->categoryWidgets[from + 10].paletteBank);
}

/* Tentative: availability bits for an equipment slot's action category. */
u32 SceneHud_GetEquipmentActionFlags(void *unused, s32 slot)
{
    u32 flags = 0;
    s32 category = sub_0822D730(slot);
    s32 id;
    u32 available;
    u32 offset;
    const struct EquipmentDescriptor *table;
    const struct EquipmentDescriptor *initialTable;
    u32 initialOffset;
    struct GameState **state;
    if (category == 3)
        return 0;
    id = Inventory_GetEquipmentId(slot);
    available = 1;
    initialTable = gUnk_08E6092C;
    initialOffset = id * sizeof(*initialTable);
    if (((const struct EquipmentDescriptor *)((const u8 *)initialTable + initialOffset))->group ==
        0) {
        table = initialTable;
        offset = initialOffset;
        state = &gGameState;
    } else {
        s16 *equipped;
        s32 remaining;
        table = initialTable;
        offset = initialOffset;
        state = &gGameState;
        equipped = (*state)->inventory;
        remaining = 3;
        do {
            if (*equipped == slot)
                available = 0;
            equipped++;
        } while (--remaining >= 0);
        if (available)
            flags |= 0x10;
    }
    if ((*state)->level >=
        ((const struct EquipmentDescriptor *)(offset + (u32)table))->requiredLevel) {
        if (category == 0)
            flags |= 1;
        else if (category == 1)
            flags |= 2;
        else if (category == 2)
            flags |= 4;
        else
            flags |= 8;
    }
    return flags;
}
