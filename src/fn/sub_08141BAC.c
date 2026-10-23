#include "global.h"

// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 94.4%
struct PlayerSmoothHistory {
    u8 unknown0[0x34C];
    s16 directionHistory[10];
};

s32 Player_GetSmoothedDirection(struct PlayerSmoothHistory *arg)
{
    struct PlayerSmoothHistory *p = arg;
    s32 direction = p->directionHistory[0];
    s32 next;
    s32 previous, i;
    const s16 *base;
    const s16 *history;
    s32 missing;
    if (direction & 1)
        return direction;
    next = (direction + 1) & 7;
    previous = (direction + 7) & 7;
    i = 1;
    base = &p->directionHistory[0];
    missing = -1;
    history = &p->directionHistory[1];
    do {
        register s32 sample asm("r2") = *history;
        register s32 result asm("r0");
        if (sample != missing) {
            result = sample;
            if (result == next || result == previous)
                return result;
            if (sample != *base)
                return direction;
        }
        history++;
    } while (++i <= 4);
    return direction;
}

#else
INCLUDE_ASM("asm/nonmatching", Player_GetSmoothedDirection);
#endif
