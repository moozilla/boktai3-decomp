// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_SelectRepeatOrExit sub_0803D700
s32 ScenePool_SelectRepeatOrExit(void *owner, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;

    if (history[0] > 4) {
        u32 mode = entry->mode;
        if (mode == 0 && entry->repetitions > 3) {
            if (entry->direction < 0) {
                entry->state = 8;
                entry->stateFrame = mode;
                entry->stateEntered = 1;
                {
                    register u32 *arg asm("r0") = history;
                    Memory_Clear(arg, 0x100);
                    asm volatile("");
                }
            } else {
                entry->state = 9;
                entry->stateFrame = mode;
                entry->stateEntered = 1;
                {
                    register u32 *arg asm("r0") = history;
                    Memory_Clear(arg, 0x100);
                }
            }
        } else {
            entry->state = 4;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            Memory_Clear(entry->history, 0x100);
            entry->repetitions++;
        }
        return 1;
    }
    return 0;
}
