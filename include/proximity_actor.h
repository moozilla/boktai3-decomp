#ifndef FOUNDRY_PROXIMITY_ACTOR_H
#define FOUNDRY_PROXIMITY_ACTOR_H
#include "global.h"
#include "player.h"
/* Tentative family: proximity-triggered state machine; identity unknown. */
struct ProximityActor {
    u8 unknown0[0x1C];
    struct SceneVec position;
    u8 unknown24[0x7C];
    struct SceneVec colliderPosition;
    u8 collider[0x54];
    u8 unknownFC[0x15];
    u8 variant;
    u8 unknown112[6];
    struct SceneVec origin;
    u8 unknown120[0x10];
    s32 previousDistanceSquared, distanceSquared, triggerDistanceSquared;
};
static inline void ProximityActor_Subtract(struct SceneVec *out, const struct SceneVec *b,
                                           const struct SceneVec *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
#endif
