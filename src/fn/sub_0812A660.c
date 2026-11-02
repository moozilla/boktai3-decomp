#include "global.h"
#include "contact_actor.h"
struct ActorEffect {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *resource;
    u8 unknown10[12];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[28];
    struct DisplaySequenceState animation;
    struct ContactCollider colliders[2];
};

// MATCHING_CONCESSION: register-pinning
struct Packed {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct Vec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};
union Vector {
    struct Packed bits;
    struct Vec halves;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetBodyProperties(void *, u16, u32, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void ActorEffect_OrientFromPositions(void);
void LinkedEffect_ApplyVariant(void);
void ActorEffect_InitColliders(struct ActorEffect *arg, u32 arg1, u32 arg2, u32 arg3, u32 extra)
{
    struct ActorEffect **actorReference;
    struct ActorEffect *p = arg;
    u32 kind = arg1;
    register u32 power asm("r9") = arg2;
    register u32 value asm("r8") = arg3;
    union Vector size;
    u32 colliderValue;
    union Vector offset;
    struct ContactCollider *primary;
    struct ContactCollider *secondary;
    struct ContactPosition *position;
    size.bits.x = 64;
    size.bits.y = 128;
    actorReference = &p;
    colliderValue = 16;
    size.bits.z = 64;
    offset.bits.x = 0;
    offset.bits.y = 0;
    offset.bits.z = 0;
    primary = &p->colliders[0];
    Collider_Init(primary, 0, 0x2001, 0, colliderValue, &size, &offset);
    Collider_SetAttackProperties(primary, kind, power, 0, value, extra);
    Collider_SetCallback(primary, ActorEffect_OrientFromPositions, p);
    position = &(*actorReference)->position;
    Collider_SetPosition(primary, position, 0);
    size.halves.x = 92;
    size.halves.y = 128;
    size.halves.z = 92;
    secondary = primary + 1;
    Collider_Init(secondary, 0, 0x4005, 0, colliderValue, &size, &offset);
    Collider_SetBodyProperties(secondary, 0, 0, 0);
    colliderValue = 0;
    Collider_SetCallback(secondary, LinkedEffect_ApplyVariant, p);
    Collider_SetPosition(secondary, position, colliderValue);
    p->colliders[1].responseValue = 60;
}
