#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: reset widths, loop invariant order, and collider pointer allocation.
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct ProjectileSprite {
    u32 flags;
    u8 pad4[0x18];
    struct ProjectilePosition position;
    u8 tail[8];
};
struct ProjectileCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8, extent;
    u8 tail[0x48];
};
struct VanargandProjectile {
    u8 entered, state, type;
    u8 pad3[3];
    u16 timer;
    u8 trailIndex, direction;
    u16 targetAngle, angle, steeringTimer, speed;
    u8 pad12[2];
    struct ProjectilePosition position, history[6];
    u8 pad4c[8];
    struct ProjectilePosition *target;
    u32 unknown58;
    u8 angles[6], radii[6];
    struct ProjectileCollider collider, healthCollider;
    struct ProjectileSprite sprites[6];
    u8 animation[0x1c];
};
struct VanargandProjectilePool {
    u8 pad0[0x18];
    u32 activeMask;
    u16 attack;
};
void AnimData_SelectPaletteBank(void *, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void AnimData_Load(void *, u32);
void Sprite_InitUnlinked(void *, void *, s32);
void CpuSet(const void *, void *, u32);
void Collider_Init(void *, u32, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
void ActorReaction_BeginOnContact(void);
void ActorReaction_Idle(void);
struct PackedVector {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
// confidence=low: initializes the six trail sprites and two projectile colliders.
#define VanargandProjectile_Initialize sub_081B6888
s32 VanargandProjectile_Initialize(struct VanargandProjectilePool *pool,
                                   struct VanargandProjectile *p, s32 index)
{
    struct ProjectileSprite *sprite;
    struct PackedVector size;
    struct PackedVector offset;
    struct VanargandProjectile *new_var;
    struct ProjectileCollider *c;
    s32 i;
    typeof(c) t1;
    p->state = 0;
    p->entered = 1;
    p->timer = 0;
    p->direction = 0;
    p->targetAngle = 0;
    AnimData_Load(p->animation, 0x8202);
    AnimData_SelectPaletteBank(p->animation, 0x1DB);
    sprite = p->sprites;
    new_var = p;
    for (i = 0; i <= 5; i++, sprite++) {
        vu32 zero;
        Sprite_InitUnlinked(sprite, new_var->animation, 1);
        *((u16 *)(((u8 *)sprite) + 0x10)) = 5 - i;
        *(((u8 *)sprite) + 7) = 1;
        do {
            zero = 0;
        } while (0);
        CpuSet((const void *)(&zero), &sprite->position, 0x05000002);
    }

    c = &p->healthCollider;
    size.x = 128;
    size.y = 512;
    size.z = 128;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(c, 0, 0x4005, 0, 16, &size, &offset);
    Collider_SetPosition(c, &p->position, 0);
    Collider_SetCallback(c, ActorReaction_BeginOnContact, p);
    Collider_SetBodyProperties(c, 0, 0, 0);
    Collider_Refresh(c);
    c = (t1 = &p->collider);
    size.x = 16;
    size.y = 100;
    size.z = 16;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(t1, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(t1, &p->position, 0);
    Collider_SetCallback(t1, ActorReaction_Idle, p);
    Collider_SetAttackProperties(t1, pool->attack, 30, 16, 0, 15);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081B6888);
#endif
