#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
// confidence=low: selects attack bounds and properties from mummy movement flags.
void Collider_SetAttackProperties(struct ContactCollider *, u32, u32, u32, u32, u32);
static inline u8 Unavailable(u8 *input)
{
    register u8 *p asm("r5") = input;
    u32 mask, off;
    if (!p)
        goto no;
    mask = 0x40;
    off = 0x2D4;
    if (*(u16 *)(p + off) & mask)
        goto yes;
    mask = 0x20000;
    off -= 4;
    if (*(u32 *)(p + off) & mask)
        goto yes;
    if (*(s16 *)(p + 0x9C) > 0)
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
static inline void ApplyAttack(u8 *base, struct ContactCollider *input, u32 alternate, u32 mask)
{
    register u8 *p asm("r5") = base;
    struct ContactCollider *collider = input;
    u32 damage;
    u32 element;
    u32 response;
    u32 flags;
    if (alternate) {
        damage = *(u16 *)(p + 0xB6);
        element = *(u16 *)(p + 0xBE);
        flags = *(u32 *)(p + 0xD0);
        {
            u32 extra = mask;
            flags |= extra;
        }
        response = *(u16 *)(p + 0xC6);
    } else {
        damage = *(u16 *)(p + 0xB4);
        element = *(u16 *)(p + 0xBC);
        flags = *(u32 *)(p + 0xCC);
        response = *(u16 *)(p + 0xC4);
    }
    asm("" : : "r"(p));
    Collider_SetAttackProperties(collider, damage, element, 0, flags, response);
}
static inline void SetMotionFlag(u8 *p, u32 mask)
{
    u32 value = mask;
    register u16 *flags asm("r2") = (void *)(p + 0x2D6);
    register u32 current asm("r1") = *flags;
    *flags = value | current;
}
void Mummy_UpdateAttackCollider(u8 *input)
{
    struct BoundsBits bounds;
    register u8 *actor asm("r5") = input;
    register struct ContactCollider *collider asm("r0");
    u32 flags;
    u32 attackBit;
    u32 attackTest;
    if (Unavailable((void *)actor))
        return;
    if (*(u16 *)(actor + 0x178))
        return;
    {
        u32 mask = 0x8000;
        flags = *(u16 *)(actor + 0x2D4);
        if (mask & flags)
            return;
    }
    attackBit = 16;
    attackTest = flags;
    if (attackTest & attackBit) {

        bounds.x = 64;
        bounds.y = 100;
        bounds.z = 64;
        collider = (struct ContactCollider *)(actor + 0x188);
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = 32;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x200004);
        SetMotionFlag(actor, 0x4000);
    } else if (flags &= 0x100) {
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = (struct ContactCollider *)(actor + 0x188);
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 1, 0x20000);
    } else {
        ((struct ContactPosition *)&bounds)->x = 32;
        ((struct ContactPosition *)&bounds)->y = 100;
        ((struct ContactPosition *)&bounds)->z = 32;
        collider = (struct ContactCollider *)(actor + 0x188);
        collider->bounds = *(struct ContactPosition *)&bounds;
        collider->contactFlags = attackBit;
        collider->flags = 0x2201;
        ApplyAttack(actor, collider, 0, 0);
    }
}
