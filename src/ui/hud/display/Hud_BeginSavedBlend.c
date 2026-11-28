#include "global.h"
#include "engine/video/render.h"
#include "engine/engine_pause_flags.h"
#include "ui/pause/scene_hud_display.h"
u32 SceneHud_CanOpenFromStart(struct SceneHudState *);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(void));
void SceneDialog_UpdateEntryFade(void);
s32 Hud_BeginSavedBlend(struct SceneHudState *hud)
{
    if (SceneHud_CanOpenFromStart(hud)) {
        Engine_SetPauseFlags(4);
        hud->savedMenuMode = gFadeMultiplier;
        hud->savedFade[0] = gBgFadeColor;
        hud->savedFade[1] = gBgFadeBankMask;
        hud->savedFade[2] = gObjFadeColor;
        hud->savedFade[3] = gObjFadeFlags;
        Hud_SetUpdateCallback(hud, SceneDialog_UpdateEntryFade);
        return 1;
    }
    return 0;
}
