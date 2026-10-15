#include "stacked_actor.h"
void Sound_PlaySong(u32);
void StackedActor_ExtendSegments(struct StackedActor *o)
{
    u16 *t = &o->frame;
    if (*t == 0)
        Sound_PlaySong(0x160);
    if (o->height <= 6) {
        o->height++;
        StackedActor_UpdateSegments(o);
    } else if (*t > 0x26) {
        StackedActor_SetUpdate(o, StackedActor_WaitForParticles);
    }
    o->frame++;
}
