#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetValueSource(u16 *source, u32 mode)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        hud->fieldValueSource = source;
        hud->fieldValueMode = mode;
    }
}
