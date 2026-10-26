// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
s32 ScenePool_SelectDirectionOrRepeat(void *owner, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    register s32 value asm("r0");
    u8 *entered;
    if (history[0] > 4) {
        s32 random;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(const u8 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
        if (random <= 127) {
            value = 1;
            entry->state = value;
            entry->stateFrame = 0;
            entered = &entry->stateEntered;
            asm volatile("");
            goto setEntered;
        }
        entry->state = 4;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        {
            register u32 *arg asm("r0") = history;
            Memory_Clear(arg, 0x100);
        }
        goto done;
    }
    if (history[1] <= 179)
        goto wait;
    if (entry->direction > 0) {
        value = 1;
        entry->state = value;
        entry->stateFrame = 0;
        entered = &entry->stateEntered;
        goto setEntered;
    }
    entry->state = 2;
    entry->stateFrame = 0;
    entered = &entry->stateEntered;
    value = 1;
setEntered:
    *entered = value;
    {
        register u32 *arg asm("r0") = history;
        Memory_Clear(arg, 0x100);
    }
done:
    return 1;
wait:
    return 0;
}
