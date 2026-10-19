#include "global.h"
/* Tentative family name; visual identity is not yet established. */
#define ActorEffect_InitColliders          sub_0812A660
#define ActorEffect_EmitCollisionParticles sub_0812A558
struct ActorEffectExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ActorEffectCollider {
    u8 pad[0x44];
    u16 unknown44;
    u8 tail[14];
};
struct ActorEffect {
    u8 pad[0x1c];
    s16 position[4];
    u8 pad24[0x58 - 0x24];
    struct ActorEffectCollider colliders[2];
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void sub_0821FF58(void *, s32, s32, s32, s32, s32);
void sub_0821FF7C(void *, s32, s32, s32);
void sub_0812A558(void);
void sub_0812A60C(void);
void sub_0812A660(struct ActorEffect *effect, s32 a, s32 b, s32 c, s32 d)
{
    struct ActorEffectExtent size, offset;
    struct ActorEffectCollider *collider;
    s16 *position;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &effect->colliders[0];
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    sub_0821FF58(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, sub_0812A558, effect);
    position = effect->position;
    Collider_SetPosition(collider, position, 0);
    size.x = 92;
    size.y = 128;
    size.z = 92;
    collider++;
    Collider_Init(collider, 0, 0x4005, 0, 16, &size, &offset);
    sub_0821FF7C(collider, 0, 0, 0);
    Collider_SetCallback(collider, sub_0812A60C, effect);
    Collider_SetPosition(collider, position, 0);
    collider->unknown44 = 60;
}
