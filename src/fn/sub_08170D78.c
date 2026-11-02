#include "scene_hud_display.h"
#include "game_state.h"
// confidence=low: refreshes inventory icons and equipped-slot markers.
#define PauseInventory_RefreshConsumableIcons sub_08170D78
struct PauseInventory {
    struct SceneHudState hud;
    u8 equippedConsumableSlots[4];
};
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_SetSpritePosition(void *, u32, s32, s32);
void PauseInventory_RefreshConsumableIcons(struct PauseInventory *p)
{
    s32 i;
    s32 markers;
    struct SceneInputWidget *widget;
    struct SceneInputWidget *equippedWidget;
    s32 consumableSlot;
    for (i = 0; i < 4; i++)
        p->equippedConsumableSlots[i] |= 255;

    markers = 0;
    i = 0;
    consumableSlot = i;
    widget = &p->hud.categoryWidgets[10];
    do {
        s32 id = Inventory_GetConsumableId(consumableSlot);
        if (id == -1)
            widget->flags |= 1;
        else {
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, p->hud.motionResource, (u16)(id + 0x121), 0);
            PauseMenu_SetSpritePalette(widget, 0x373);
            if (GameState_IsConsumableSlotEquipped(consumableSlot)) {
                s32 marker = markers + 30;
                PauseMenu_ClearSpriteFlags(p, marker, 1);
                PauseMenu_SetSpritePosition(p, marker, widget->position.x, widget->position.y);
                p->equippedConsumableSlots[markers] = consumableSlot;
                markers++;
            }
        }
        widget++;
        consumableSlot++;
    } while (consumableSlot <= 15);
    for (; markers <= 15; markers++)
        PauseMenu_AddSpriteFlags(p, markers + 30, 1);

    i = 0;
    equippedWidget = &p->hud.categoryWidgets[26];
    for (i = 0; i < 4; i++) {
        struct GameState *state = gGameState;
        s16 *selected = &((struct GameState *)(i * 2 + (u32)state))->party[0];
        s32 id;
        if (*selected < 0) {
            PauseMenu_AddSpriteFlags(p, i + 26, 1);
        } else {
            id = Inventory_GetConsumableId(*selected);
            if (id < 0) {
                PauseMenu_AddSpriteFlags(p, i + 26, 1);
            } else {
                PauseMenu_ClearSpriteFlags(p, i + 26, 1);
                PauseMenu_SetSpritePosition(p, i + 26, (i * 24) + 8, 104);
                Anim_LoadKeyframe(equippedWidget, p->hud.motionResource, (u16)(id + 0x121), 0);
                PauseMenu_SetSpritePalette(equippedWidget, 0x373);
            }
        }
        equippedWidget++;
    }
}
