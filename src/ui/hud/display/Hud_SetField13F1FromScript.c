#include "global.h"

#include "engine/engine.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetField13F1FromScript(void)
{
    if (gFieldHud && Script_SeekToKeyword('f'))
        ((struct SceneHudState *)gFieldHud)->secondaryInputEnabled = Script_GetValue();
}
