#include "sprite_trail.h"
struct PackedCoordinate {
    u32 lo : 16;
    u32 hi : 16;
};
struct ColliderVector {
    struct PackedCoordinate xy, z;
};
void SingletonSlot7_OnEffectMotionComplete(void *);
void SpatialIndex_InitEntry(void *, struct ColliderVector *, struct ColliderVector *,
                            struct ColliderVector *, u32, u32, u32, u32, u32, void (*)(void *),
                            void *);
void SpriteTrail_InitCollider(struct SpriteTrail *p)
{
    struct ColliderVector position, size, offset;
    void *collider = p->collider;
    u32 kind = 5;
    position.xy.lo = p->position.x >> 12;
    position.xy.hi = p->position.y >> 12;
    position.z.lo = p->position.z >> 12;
    size.xy.lo = 4;
    size.xy.hi = 4;
    size.z.lo = 4;
    *(u32 *)&offset.xy = 0;
    *(u32 *)&offset.z &= 0xFFFF0000;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 12, kind, 1, p->collisionArg0,
                           p->collisionArg1, SingletonSlot7_OnEffectMotionComplete, p);
}
