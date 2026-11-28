#include "global.h"
#include "monsters/popbud/popbud_playback.h"
#include "engine/engine.h"

void Combat_ApplyHitReaction(void);
void Collider_SetBodyProperties(void *, u32, u32, u32);

void Popbud_InitCollider(struct PinkSpineState *popbud)
{
    struct ContactCollider *collider = &popbud->collider;
    struct ContactPosition bounds, offset;

    bounds.x = 100;
    bounds.y = 120;
    bounds.z = 100;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &popbud->sprite.position, 0);
    Collider_SetCallback(collider, Combat_ApplyHitReaction, popbud);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
