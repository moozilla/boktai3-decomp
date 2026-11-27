#include "global.h"
#include "npc/story_character/state_actor.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Kuro_BlendPalette(struct StateActor *, s32, s32);
void Kuro_OscillatePaletteBlend(void *ctx, struct StateActor *p, s32 ticks)
{
    s32 phase;
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
    }
    if (Div(ticks, 60) & 1)
        phase = 60 - Mod(ticks, 60);
    else
        phase = Mod(ticks, 60);
    Kuro_BlendPalette(p, phase, 60);
}
