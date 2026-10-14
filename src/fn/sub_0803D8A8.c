// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_ResumeAfterIdle sub_0803D8A8
void sub_08219DD8(void *, s32);
s32 ScenePool_ResumeAfterIdle(u8 *owner, struct ScenePoolEntry *entry)
{
    if ((s8)owner[0x5ec] <= 0 && entry->idleFrame > 79) {
        s32 zero = 0;
        entry->idleFrame = zero;
        if (entry->direction > 0) {
            register s32 one asm("r0") = 1;
            entry->state = one;
            entry->stateFrame = zero;
            entry->stateEntered = one;
            sub_08219DD8(entry->history, 0x100);
        } else {
            entry->state = 2;
            entry->stateFrame = zero;
            entry->stateEntered = 1;
            sub_08219DD8(entry->history, 0x100);
        }
        return 1;
    }
    return 0;
}
