#include "stacked_actor.h"
#define StackedActor_ExtendForScript sub_08052CE4
void Sound_PlaySong(u32);
void StackedActor_ExtendForScript(struct StackedActor *p)
{
    u16 *c = &p->height;
    if (*c <= 6) {
        *c += 1;
        sub_08052418(p);
    } else {
        u16 *d = &p->frame;
        u32 v = *d + 1;
        *d = v;
        if ((u16)v > 0x1f) {
            Sound_PlaySong(0x15f);
            sub_08052628(p, sub_08052D34);
        }
    }
}
