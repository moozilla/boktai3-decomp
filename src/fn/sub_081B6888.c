#include "global.h"
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct ProjectileSprite {
    u32 flags;
    u8 pad4[3];
    u8 unknown7;
    u8 pad8[8];
    u16 trailOrder;
    u8 pad12[10];
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
static inline u16 VanargandProjectile_GetResetValue(void)
{
    return 0;
}
static inline void VanargandProjectile_FillPosition(u32 fill, struct ProjectilePosition *pos)
{
    vu32 zero = fill;
    CpuSet((const void *)&zero, pos, 0x05000002);
}
// confidence=low: initializes the six trail sprites and two projectile colliders.
#define VanargandProjectile_Initialize sub_081B6888
s32 VanargandProjectile_Initialize(struct VanargandProjectilePool *pool,
                                   struct VanargandProjectile *p, s32 index)
{
    struct ProjectileSprite *sprite;
    struct PackedVector size, offset;
    struct ProjectileCollider *c;
    s32 i;
    u16 reset = VanargandProjectile_GetResetValue();
    p->state = 0;
    p->entered = 1;
    p->timer = reset;
    p->direction = 0;
    p->targetAngle = reset;
    AnimData_Load(p->animation, 0x8202);
    AnimData_SelectPaletteBank(p->animation, 0x1DB);
    sprite = p->sprites;
    for (i = 0; i <= 5; i++, sprite++) {
        Sprite_InitUnlinked(sprite, p->animation, 1);
        sprite->trailOrder = 5 - i;
        sprite->unknown7 = 1;
        VanargandProjectile_FillPosition(0, &sprite->position);
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
    c = &p->collider;
    size.x = 16;
    size.y = 100;
    size.z = 16;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(c, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(c, &p->position, 0);
    Collider_SetCallback(c, ActorReaction_Idle, p);
    Collider_SetAttackProperties(c, pool->attack, 30, 16, 0, 15);
}
