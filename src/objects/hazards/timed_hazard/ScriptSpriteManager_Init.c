#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

s32 ScriptSpriteManager_Init(struct TimedHazardPool *pool, void *unused)
{
    gColliderSpritePool = (u32)pool;
    pool->animation = ScriptProc_Start(0x922E, 0x931E);
    pool->head = 0;
    return 0;
}
