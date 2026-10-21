#include "global.h"
#include "scene_hud_display.h"
// MATCHING_CONCESSION: register-pinning
void Hud_SetInteractionCallback(void *, void (*)(void), int);
s32 SceneHud_TickPaletteBlend(void *);
void SceneHud_RefillActionStatB(void);
void SceneHud_TickActionResult(struct SceneHudState *s)
{
    u32 frame;
    if (s->actionTransferDelay > 7)
        SceneHud_TickPaletteBlend(s);
    if (!s->actionTransferBonus) {
        if (s->actionTransferDelay > 9)
            Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    } else {
        Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    }
    frame = s->actionTransferDelay;
    {
        s32 mask = 7;
        s32 phase;
        asm("" : "+r"(mask));
        phase = mask;
        phase &= frame;
        if (phase < mask - (s32)(frame >> 3))
            s->motionWidgets[14].flags |= 1;
        else
            s->motionWidgets[14].flags &= ~1;
    }
    if (++s->actionTransferDelay > 55)
        Hud_SetInteractionCallback(s, SceneHud_RefillActionStatB, 1);
}
