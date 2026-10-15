#include "stacked_actor.h"
void Sound_PlaySong(u32);
void StackedActor_ExtendForScript(struct StackedActor *p)
{
    u16 *c = &p->height;
    if (*c <= 6) {
        *c += 1;
        StackedActor_UpdateSegments(p);
    } else {
        u16 *d = &p->frame;
        u32 v = *d + 1;
        *d = v;
        if ((u16)v > 0x1f) {
            Sound_PlaySong(0x15f);
            StackedActor_SetUpdate(p, StackedActor_WaitForScriptParticles);
        }
    }
}
