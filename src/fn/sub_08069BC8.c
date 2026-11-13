#include "burst_emitter.h"
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
struct BurstEmitterSize {
    u32 x : 16, y : 16, z : 16;
};
void BurstEmitter_InitCollider(struct BurstEmitter *emitter, s32 a, s32 b, s32 c, s32 config)
{
    struct BurstEmitterSize size, offset;
    struct ContactCollider *collider = &emitter->collider;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)config, &size, &offset);
    if (!emitter->context)
        Collider_SetAttackProperties(collider, a, b, 512, 0x240004, c);
    else
        Collider_SetAttackProperties(collider, a, b, 512, 0x40000, c);
    collider->collisionId = 0x3246;
    Collider_SetPosition(collider, &emitter->position, 0);
    Collider_SetCallback(collider, NULL, emitter);
}
