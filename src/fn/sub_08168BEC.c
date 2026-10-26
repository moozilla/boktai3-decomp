#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
s32 SolarFruit_GetInventorySlot(s32);
void SceneHud_RebuildTertiaryOptions(struct SceneHudState *p)
{
    s32 count = 0;
    s32 i;
    struct SceneHudTertiaryOption *option;
    struct GameState **state;
    p->tertiarySelection = -1;
    i = 0;
    state = &gGameState;
    option = p->tertiaryOptions;
    for (; i <= 7; i++) {
        u8 *q = (u8 *)*state;
        s16 *slot;
        slot = (s16 *)(i * 2 + (u32)q);
        slot += 0x48;
        if (*slot >= 0) {
            s32 icon = SolarFruit_GetInventorySlot(*slot);
            if (icon != -1) {
                if (i == (*state)->actionAuxiliarySlot)
                    p->tertiarySelection = (*state)->actionAuxiliarySlot;
                option->icon = icon;
                option->slot = i;
                option++;
                count++;
            }
        }
    }
    if (p->tertiarySelection == -1) {
        p->tertiarySelection = 0;
        gGameState->actionAuxiliarySlot = 0;
    }
    p->tertiaryVisible = count;
}
