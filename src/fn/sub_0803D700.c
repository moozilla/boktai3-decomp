#include "scene_pool.h"
static inline void ScenePool_ClearHistory(u32 bytes, u32 *ptr)
{
    Memory_Clear(ptr, bytes);
}
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
                ScenePool_ClearHistory(sizeof(entry->history), history);
            } else {
                entry->state = 9;
                entry->stateFrame = mode;
                entry->stateEntered = 1;
                ScenePool_ClearHistory(sizeof(entry->history), history);
            }
            return 1;
        }
        entry->state = 4;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        Memory_Clear(entry->history, 0x100);
        entry->repetitions++;
        return 1;
    }
    return 0;
}
