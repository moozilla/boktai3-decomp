#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

s32 ColliderSpritePool_Update(struct TimedHazardPool *);
u32 ScriptSpriteManager_Destroy(struct TimedHazardPool *);
s32 ScriptSpriteManager_Init(struct TimedHazardPool *, void *);

struct TimedHazardPool *ColliderSpritePool_Create(void *argument)
{
    struct TimedHazardPool *pool = Task_Create(8, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))ColliderSpritePool_Update,
                          (void (*)(void *))ScriptSpriteManager_Destroy);
        if (ScriptSpriteManager_Init(pool, argument) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
