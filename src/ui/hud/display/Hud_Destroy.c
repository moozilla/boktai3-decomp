#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void PauseMenu_RemoveHeaderAnimation(struct SceneHudState *);
void PauseMenu_RemoveEquipmentSlotAnimations(struct SceneHudState *);
void Anim_Remove(void *);
s32 Hud_Destroy(struct SceneHudState *hud)
{
    PauseMenu_RemoveHeaderAnimation(hud);
    PauseMenu_RemoveEquipmentSlotAnimations(hud);
    Anim_Remove(&hud->primaryWidgets[0]);
    Anim_Remove(&hud->secondaryWidgets[0]);
    gFieldHud = 0;
    return 0;
}
