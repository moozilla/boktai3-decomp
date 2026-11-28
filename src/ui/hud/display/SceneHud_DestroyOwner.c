#include "global.h"
#include "ui/pause/scene_hud_display.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
void SceneHud_RemoveAnimations(struct SceneHudState *);
void PauseMenu_RemoveHeaderAnimation(struct SceneHudState *);
void PlayerPanel_RemoveEffects(struct SceneHudState *);
void Hud_RemoveMenuGraphics(struct SceneHudState *);
void PauseMenu_RemoveEquipmentSlotAnimations(struct SceneHudState *);
static inline u8 OwnerWidgetsEnabled(u32 mask)
{
    if ((gGameState->flags868 & mask))
        return 1;
    return 0;
}
s32 SceneHud_DestroyOwner(struct SceneHudState *s)
{
    s32 i;
    SceneHud_RemoveAnimations(s);
    PauseMenu_RemoveHeaderAnimation(s);
    PlayerPanel_RemoveEffects(s);
    Hud_RemoveMenuGraphics(s);
    PauseMenu_RemoveEquipmentSlotAnimations(s);
    if (OwnerWidgetsEnabled(0x80) && s->initializationFlag) {
        for (i = 0; i < 2; i++) {
            if (s->fieldWidgets.meterWidgets[i].active)
                Anim_Remove(&s->fieldWidgets.meterWidgets[i]);
        }
        for (i = 0; i < 6; i++) {
            if (s->fieldWidgets.timeDigits[i].active)
                Anim_Remove(&s->fieldWidgets.timeDigits[i]);
        }
    }
    gSceneHudMode = s->mode;
    gFieldHud = 0;
    return 0;
}
