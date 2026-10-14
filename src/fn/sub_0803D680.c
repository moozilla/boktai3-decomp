// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_SelectInitialDirection sub_0803D680
s32 ScenePool_SelectInitialDirection(void *owner, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    if (history[0] > 4) {
        s32 random;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
        random = *(const u8 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
        if (random <= 127)
            goto forward;
        goto backward;
    }
    if (history[1] <= 179)
        goto wait;
    if (entry->direction > 0) {
    forward: {
        register s32 one asm("r0") = 1;
        s32 zero;
        entry->state = one;
        zero = 0;
        entry->stateFrame = zero;
        entry->stateEntered = one;
        {
            register u32 *arg asm("r0") = history;
            Memory_Clear(arg, 0x100);
        }
    }
    } else {
    backward:
        entry->state = 2;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        {
            register u32 *arg asm("r0") = history;
            Memory_Clear(arg, 0x100);
        }
    }
    return 1;
wait:
    return 0;
}
