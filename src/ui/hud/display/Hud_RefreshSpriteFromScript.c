#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetupBackgroundLayer(struct SceneHudState *, u32, s32, s32, u32);
void Hud_RefreshSpriteFromScript(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud)
        Hud_SetupBackgroundLayer(hud, hud->panelResource, 0, 0, hud->panelConfig);
}
