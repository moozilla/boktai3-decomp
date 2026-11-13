#ifndef GUARD_REGISTRY_ACTOR_COLLISION_H
#define GUARD_REGISTRY_ACTOR_COLLISION_H
#include "registry_actor.h"
#include "contact_actor.h"
#include "input.h"
struct RegistryCollisionMessage {
    u32 unknown0;
    s32 type, kind;
    u16 value, unknownE;
    struct ContactPosition position;
    u8 unknown18[8];
    struct ContactPosition offset;
    u8 unknown28[0xE];
    u8 speed;
};
struct RegistryIntegerVector { s32 x, y, z, w; };
struct RegistryUnsignedVector { u32 x : 16, y : 16, z : 16, pad : 16; };
union RegistryCollisionVector {
    struct ContactPackedVector packed;
    struct RegistryUnsignedVector unsignedPacked;
    s16 components[4];
    u16 unsignedComponents[4];
};
void Actor_ApplyKnockback(struct RegistryActor *, struct RegistryCollisionMessage *);
void ActorMotion_SetKnockback(struct RegistryActor *, struct RegistryCollisionMessage *);
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32,
    void (*)(struct RegistryActor *, struct RegistryCollisionMessage *), void *);
void SpatialIndex_InsertEntry(void *);
s8 SpriteSlots_AttachSource(void *);
void TimedSpriteEffect_RequestStop(u32);
#endif
