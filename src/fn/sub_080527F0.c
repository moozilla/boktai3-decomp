#include "stacked_actor.h"
#define StackedActor_ExtendSegments sub_080527F0
void Sound_PlaySong(u32);
void StackedActor_ExtendSegments(struct StackedActor *o)
{
    u16 *t = &o->frame;
    if (*t == 0)
        Sound_PlaySong(0x160);
    if (o->height <= 6) {
        o->height++;
        sub_08052418(o);
    } else if (*t > 0x26) {
        sub_08052628(o, sub_0805284C);
    }
    o->frame++;
}
