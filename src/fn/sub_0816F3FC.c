#include "global.h"
#include "scene_hud_display.h"
void Hud_SetInteractionCallback(void *, void (*)(void), int);
s32 SceneHud_TickPaletteBlend(void *);
void SceneHud_RefillActionStatB(void);
void SceneHud_TickActionResult(struct SceneHudState *s)
{
    if (s->actionTransferDelay > 7)
        SceneHud_TickPaletteBlend(s);
    if (!s->actionTransferBonus) {
        if (s->actionTransferDelay > 9)
            Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    } else {
        Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    }
    if ((s->actionTransferDelay & 7) < 7 - (s->actionTransferDelay >> 3))
        s->motionWidgets[14].flags |= 1;
    else
        s->motionWidgets[14].flags &= ~1;
    if (++s->actionTransferDelay > 55)
        Hud_SetInteractionCallback(s, SceneHud_RefillActionStatB, 1);
}
