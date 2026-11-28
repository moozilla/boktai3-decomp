#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"

void Hud_SetMode(u32);
/* Re-applies the control mode saved by Hud_SaveMode, if any, and clears the
   saved copy (-1). */
void Hud_RestoreMode(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud && hud->previousPage >= 0) {
        Hud_SetMode(hud->previousPage);
        hud->previousPage = -1;
    }
}
