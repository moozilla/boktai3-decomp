#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
#include "scene_actor_chase_shared.h"
// confidence=low: selects attack bounds and properties from mummy movement flags.
void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{

    u32 mask;
    if (!actor)
        goto no;
    mask = 0x40;
    if (actor->flags2D4 & mask)
        goto yes;
    mask = 0x20000;
    if (actor->flags2D0 & mask)
        goto yes;
    if (actor->health > 0)
        goto no;
yes:
    return 1;
no:
    return 0;
}

struct BoundsBits {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
static inline void ApplyAttack(struct SceneActorRegistryResetView *actor,
                               struct ContactCollider *collider, u32 alternate, u32 mask)
{

    u32 damage;
    u32 element;
    u32 response;
    u32 flags;
    if (alternate) {
        damage = actor->attacks.first[1];
        element = actor->attacks.second[1];
        flags = actor->attacks.flags[1];
        {
            u32 extra = mask;
            flags |= extra;
        }
        response = actor->attacks.parameter[1];
    } else {
        damage = actor->attacks.first[0];
        element = actor->attacks.second[0];
        flags = actor->attacks.flags[0];
        response = actor->attacks.parameter[0];
    }
    Collider_SetAttackProperties(collider, damage, element, 0, flags, response);
}
void Mummy_UpdateAttackCollider(u8 *input)
{
    struct BoundsBits bounds;
    struct SceneActorRegistryResetView *actor = (void *)input;
    register struct ContactCollider *collider asm("r0");
    u32 flags;
    u32 attackBit;

    if (Unavailable(actor))
        return;
    if (actor->colliderA.responseValue)
        return;
    {
        u32 mask = 0x8000;
        flags = actor->flags2D4;
        if (mask & flags)
            return;
    }
    attackBit = 16;
    if (flags & attackBit) {

        bounds.x = 64;
        bounds.y = 100;
        bounds.z = 64;
        collider = &actor->colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = 32;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x200004);
        SceneActorChase_SetReactionFlags(actor, 0x4000);
    } else if (flags & 0x100) {
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = &actor->colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x20000);
    } else {
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = &actor->colliderB;
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 0, 0);
    }
}
