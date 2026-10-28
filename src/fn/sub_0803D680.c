#include "scene_pool.h"
static inline void ClearHistory(int size, u32 *history)
{
    Memory_Clear(history, size);
}
s32 ScenePool_SelectInitialDirection(void *owner, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    if (history[0] > 4) {
        s32 random;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(const u8 *)((gRandomIndex << 1) + (u32)gRandomTable);
        if (random <= 127)
            goto forward;
        goto backward;
    }
    if (history[1] <= 179)
        goto wait;
    if (entry->direction > 0) {
    forward: {
        s32 one = 1;
        s32 zero;
        entry->state = 1;
        zero = 0;
        entry->stateFrame = zero;
        entry->stateEntered = one;
        {
            u32 *arg = history;
            ClearHistory(0x100, arg);
        }
    }
    } else {
    backward:
        entry->state = 2;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        {
            u32 *arg = history;
            ClearHistory(0x100, arg);
        }
    }
    return 1;
wait:
    return 0;
}
