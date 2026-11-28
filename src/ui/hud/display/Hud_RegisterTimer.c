#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_RegisterTimer(u16 *timer)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        hud->countdownValue = timer;
        hud->countdownEnabled = 1;
        if (gEngineFlags & 0x800)
            hud->actionCueInitialized = 1;
    }
}
