#include "tether_actor.h"
#define TetherActor_SetHeadingEntry sub_08049180
s32 DisplayAnimation_SetEntry(struct DisplayAnimation *, u32, u32, u32, u32, u32, u32);
s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *);
s32 TetherActor_SetHeadingEntry(struct TetherActor *actor, u32 entry)
{
    u32 variant, flags;
    struct DisplayAnimation *display;
    if (actor->heading > 4) {
        variant = 8 - actor->heading;
        flags = 1;
    } else {
        variant = actor->heading;
        flags = 0;
    }
    display = &actor->display.animation;
    DisplayAnimation_SetEntry(display, entry, variant, flags, 0, 0, 0);
    return DisplayAnimation_UpdatePlayback(display);
}
