#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"

s32 sub_08177D88(void)
{
    if (gFieldHud == NULL)
        return -1;
    return ((struct SceneHudState *)gFieldHud)->initializationState;
}
