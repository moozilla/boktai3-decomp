#include "scene_pool.h"

s32 ScenePool_ResumeAfterIdle(struct ScenePoolActor *owner, struct ScenePoolEntry *entry)
{
    if (owner->motionLock <= 0 && entry->idleFrame > 79) {
        entry->idleFrame = 0;
        if (entry->direction > 0) {
            ScenePool_EnterStateAndClearHistory(entry, 1);
        } else {
            ScenePool_EnterStateAndClearHistory(entry, 2);
        }
        return 1;
    }
    return 0;
}
