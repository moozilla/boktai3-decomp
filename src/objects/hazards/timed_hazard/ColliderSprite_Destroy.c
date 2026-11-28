#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"

s32 ColliderSprite_Destroy(struct TimedHazard *hazard)
{
    Sprite_Remove(&hazard->sprite);
    if (gColliderSpritePool)
        TimedHazard_UnlinkInstance((struct TimedHazardPool *)gColliderSpritePool, hazard);
    if (hazard->collider) {
        Collider_Remove(hazard->collider);
        Heap_Free(hazard->collider);
    }
    if (hazard->attack)
        Heap_Free(hazard->attack);
    Heap_Free(hazard);
#ifdef UBFIX
    // falls off the end with Heap_Free's leftover in r0; no caller reads it
    return 0;
#endif
}
