#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetPanelIndexFromScript(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        if (Script_SeekToKeyword('i'))
            hud->scriptPanelIndex = Script_GetValue();
    }
}
