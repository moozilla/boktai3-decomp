#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void InventoryPanel_StartCallbackBurst(struct SceneHudState *, s32, s32, s32);
void Sound_PlaySong(u32);
void Hud_BeginModeSixCue(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud && !hud->initializationState && hud->controlMode <= 2) {
        InventoryPanel_StartCallbackBurst(hud, 0, 0xE0, 0x90);
        hud->callbackBurstTimer = 6;
        Sound_PlaySong(0x382);
    }
}
