#include "global.h"


void ActorContact_TickAuxiliaryTimers(u8 *actor)
{
    u8 *context = *(u8 **)(actor + 0x3D0);
    u32 *flags = (u32 *)(context + 0x3D4);
    u8 *entry = context;
    u32 offset = 0x430;
    s32 i;
    i = 3;
loop: {
    u16 *timer = (u16 *)(entry + offset);
    if (*timer) {
        --*timer;
        if (*timer == 0)
            *flags |= 1;
    }
    flags = (u32 *)((u8 *)flags + 0x60);
    entry += 0x60;
}
    if (--i >= 0)
        goto loop;
}
