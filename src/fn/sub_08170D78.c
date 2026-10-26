#include "global.h"
#include "game_state.h"
// confidence=low: refreshes inventory icons and equipped-slot markers.
#define PauseInventory_RefreshConsumableIcons sub_08170D78
// MATCHING_CONCESSION: register-pinning
struct PauseMarker {
    u8 slot;
} __attribute__((packed));
struct PauseInventory {
    u8 unknown[0x4D98];
    struct PauseMarker markers[4];
};
struct EquippedSlot {
    u8 unknown[0x58];
    s16 id;
};
struct PauseWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z, pad;
    u8 unknown28[0x38];
};
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_SetSpritePosition(void *, u32, s32, s32);
void PauseInventory_RefreshConsumableIcons(u8 *p)
{
    s32 i;
    s32 markers;
    struct PauseWidget *widget;
    struct PauseWidget *equippedWidget;
    s32 consumableSlot;
    for (i = 0; i < 4; i++)
        ((struct PauseInventory *)p)->markers[i].slot |= 255;

    markers = 0;
    i = 0;
    consumableSlot = i;
    widget = (void *)(p + 0x17E0);
    do {
        s32 id = Inventory_GetConsumableId(consumableSlot);
        if (id == -1)
            widget->flags |= 1;
        else {
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, p + 0xA4, (u16)(id + 0x121), 0);
            PauseMenu_SetSpritePalette(widget, 0x373);
            if (GameState_IsConsumableSlotEquipped(consumableSlot)) {
                s32 marker = markers + 30;
                PauseMenu_ClearSpriteFlags(p, marker, 1);
                PauseMenu_SetSpritePosition(p, marker, widget->x, widget->y);
                ((struct PauseInventory *)p)->markers[markers].slot = consumableSlot;
                markers++;
            }
        }
        widget++;
        consumableSlot++;
    } while (consumableSlot <= 15);
    for (; markers <= 15; markers++)
        PauseMenu_AddSpriteFlags(p, markers + 30, 1);

    i = 0;
    equippedWidget = (void *)(p + 0x1DE0);
    for (i = 0; i < 4; i++) {
        u8 *state = gGameState;
        struct EquippedSlot *slot = (void *)(i * 2 + (u32)state);
        s32 id;
        s32 slotId = slot->id;
        // Keep the validity read separate from the subsequent slot lookup.
        asm("" : "+r"(slotId));
        if ((slotId < 0) || ((id = Inventory_GetConsumableId(slot->id)) < 0)) {
            PauseMenu_AddSpriteFlags(p, i + 26, 1);
        } else {
            PauseMenu_ClearSpriteFlags(p, i + 26, 1);
            PauseMenu_SetSpritePosition(p, i + 26, (i * 24) + 8, 104);
            Anim_LoadKeyframe(equippedWidget, p + 0xA4, (u16)(id + 0x121), 0);
            PauseMenu_SetSpritePalette(equippedWidget, 0x373);
        }
        equippedWidget++;
    }
}
