#include "scene_pool.h"
static inline void ScenePool_ClearHistory(int size, u32 *history)
{
    Memory_Clear(history, size);
}
s32 ScenePool_SelectDirectionOrRepeat(void *owner, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    if (history[0] > 4) {
        s32 random;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(const u8 *)((gRandomIndex << 1) + (u32)gRandomTable);
        if (random <= 127) {
            entry->state = 1;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            ScenePool_ClearHistory(sizeof(entry->history), history);
        } else {
            entry->state = 4;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            ScenePool_ClearHistory(sizeof(entry->history), history);
        }
        return 1;
    }
    if (history[1] > 179) {
        if (entry->direction > 0) {
            entry->state = 1;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            ScenePool_ClearHistory(sizeof(entry->history), history);
            return 1;
        }
        entry->state = 2;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        ScenePool_ClearHistory(sizeof(entry->history), history);
        return 1;
    }
    return 0;
}
