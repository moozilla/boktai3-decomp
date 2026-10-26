#include "global.h"
#include "scene_hud_display.h"
#include "ram.h"
#define gSceneHudState gFieldHud
void SceneHud_SetTimedState(u32 unused, s32 start, u32 value, u32 kind, u32 phaseEnabled,
                            u32 parameter)
{
    struct SceneHudState *p = gFieldHud;
    if (p && !p->controlMode) {
        if (start < 0)
            p->timedStateStart = 0;
        else
            p->timedStateStart = start;
        p->timedStateValue = value;
        p->timedStateKind = kind;
        p->timedStateTimer = 300;
        p->timedStateParameter = parameter;
        if (phaseEnabled)
            p->timedStatePhase = 6;
        else
            p->timedStatePhase = 0;
    }
}
