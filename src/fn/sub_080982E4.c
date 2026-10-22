#include "global.h"

// MATCHING_CONCESSION: register-pinning

void ActorContact_TickAuxiliaryTimers(u8 *actor)
{
    u8 *context = *(u8 **)(actor + 0x3D0);
    u32 *flags = (u32 *)(context + 0x3D4);
    u8 *entry = context;
    u32 offset = 0x430;
    s32 i;
    asm("" : "+r"(offset));
    i = 3;
    do {
        u16 *timer = (u16 *)(entry + offset);
        if (*timer) {
            --*timer;
            if (*timer == 0) {
                u32 value = *flags;
                register u32 bit asm("r1") = 1;
                asm("" : "+r"(bit));
                *flags = value | bit;
            }
        }
        flags = (u32 *)((u8 *)flags + 0x60);
        entry += 0x60;
    } while (--i >= 0);
}
