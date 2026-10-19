#include "global.h"
#include "scene_hud_display.h"
#ifdef NONMATCHING
// fuzzy: 97.9%
// foundry draft (worker best 97.9%); what differs is mostly register allocation and instruction order
#define SceneHud_TickActionResult sub_0816F3FC
#define SceneHud_TickPaletteBlend sub_08165D10
void Hud_SetInteractionCallback(void *, void (*)(void), int);
s32 sub_08165D10(void *);
void SceneHud_RefillActionStatB(void);
void sub_0816F3FC(struct SceneHudState *s)
{
    u32 frame;
    if (s->actionTransferDelay > 7)
        sub_08165D10(s);
    if (!s->actionTransferBonus) {
        if (s->actionTransferDelay > 9)
            Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    } else {
        Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    }
    frame = s->actionTransferDelay;
    {
    s32 mask = 7;
    s32 phase = mask;
    phase &= frame;
    if (phase < mask - (s32)(frame >> 3))
        s->motionWidgets[14].flags |= 1;
    else
        s->motionWidgets[14].flags &= ~1;
    }
    if (++s->actionTransferDelay > 55)
        Hud_SetInteractionCallback(s, SceneHud_RefillActionStatB, 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816F3FC);
#endif
