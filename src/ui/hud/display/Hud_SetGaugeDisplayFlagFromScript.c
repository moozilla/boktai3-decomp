#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetGaugeDisplayFlagFromScript(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        if (Script_SeekToKeyword('f'))
            hud->playerValueEnabled = Script_GetValue();
    }
}
