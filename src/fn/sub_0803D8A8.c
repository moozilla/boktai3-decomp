#include "scene_pool.h"
s32 ScenePool_ResumeAfterIdle(u8 *owner, struct ScenePoolEntry *entry)
{
    if ((s8)owner[0x5ec] <= 0 && entry->idleFrame > 79) {
        s32 zero = 0;
        entry->idleFrame = zero;
        if (entry->direction > 0) {
            s32 one = 1;
            do { /* scope block steers register allocation */
                entry->state = one;
            } while (0);
            entry->stateFrame = zero;
            entry->stateEntered = one;
            Memory_Clear(entry->history, 0x100);
        } else {
            entry->state = 2;
            entry->stateFrame = zero;
            entry->stateEntered = 1;
            Memory_Clear(entry->history, 0x100);
        }
        return 1;
    }
    return 0;
}
