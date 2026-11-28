#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetMode(u32);
/* Script: stops the message display timer and sets HUD mode seven (used when
   the message window closes). */
void Hud_CloseMessageFromScript(void)
{
    if (gFieldHud) {
        ((struct SceneHudState *)gFieldHud)->messageDisplayTimer = 0;
        Hud_SetMode(7);
    }
}
