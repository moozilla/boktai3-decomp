// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
s32 DisplayAnimation_SetEntry(struct DisplayAnimation *, u32, u32, u32, u32, u32, u32);
s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *);
s32 TetherActor_SetCardinalEntry(struct TetherActor *actor, u32 unusedEntry)
{
    s32 angle = (u8)((actor->heading + 5) << 5);
    u8 flags[2];
    struct DisplayAnimation *display;
    u32 orientation;
    orientation = ((angle + 96) >> 6) & 3;
    flags[0] = orientation;
    {
        u8 *packet = flags;
        register u8 *saved asm("r0") = packet;
        u8 *first;
        u8 *second;
        u8 *savedSecond;
        register u32 sector asm("r3");
        u8 test;
        sector = *saved;
        first = saved;
        second = packet + 1;
        test = sector - 1;
        savedSecond = second;
        if (test <= 1)
            *first = 1;
        else
            *first = 0;
        if (sector > 1)
            *second = 1;
        else
            *second = 0;
        display = &actor->display.animation;
        DisplayAnimation_SetEntry(display, 44, flags[0], *savedSecond, 0, 0, 0);
    }
    return DisplayAnimation_UpdatePlayback(display);
}
