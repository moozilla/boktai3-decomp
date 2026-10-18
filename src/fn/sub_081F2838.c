#include "global.h"
struct PackedCoordinate {
    u32 lo : 16;
    u32 hi : 16;
};
struct ColliderVector {
    struct PackedCoordinate xy, z;
};
struct SpriteTrail {
    u32 unknown0;
    s32 position[4];
    u8 pad14[0x64];
    u32 collisionArg0, collisionArg1;
    u8 pad80[8];
    u8 collider[0x40];
};
void MotionEffectPool_OnRenderComplete(void *);
void SpatialIndex_InsertEntry(void *);
void SpatialIndex_InitEntry(void *, struct ColliderVector *, struct ColliderVector *,
                            struct ColliderVector *, u32, u32, u32, u32, u32, void (*)(void *),
                            void *);
void SpriteTrail_InitRenderCollider(struct SpriteTrail *p)
{
    struct ColliderVector position, size, offset;
    void *collider = (u8 *)p + 0xc8;
    u32 kind = 5;
    position.xy.lo = p->position[0] >> 12;
    position.xy.hi = p->position[0] >> 12;
    position.z.lo = p->position[0] >> 12;
    size.xy.lo = 4;
    size.xy.hi = 4;
    size.z.lo = 4;
    *(u32 *)&offset.xy = 0;
    *(u32 *)&offset.z &= 0xFFFF0000;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 20, kind, 1, 0, 0,
                           MotionEffectPool_OnRenderComplete, p);
    SpatialIndex_InsertEntry(collider);
}
