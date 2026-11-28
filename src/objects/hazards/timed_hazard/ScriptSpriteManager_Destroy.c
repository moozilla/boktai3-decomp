#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

u32 ScriptSpriteManager_Destroy(struct TimedHazardPool *pool)
{
    struct TimedHazard *p = pool->head;
    while (p) {
        struct TimedHazard *next = p->next;
        ColliderSprite_Destroy(p);
        p = next;
    }
    return (u32)(gColliderSpritePool = 0);
}
