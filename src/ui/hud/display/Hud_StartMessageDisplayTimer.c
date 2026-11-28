#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_StartMessageDisplayTimer(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud)
        hud->messageDisplayTimer = 90;
}
