#include "global.h"
struct ProjectileVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ProjectileCollider {
    u8 pad00[6];
    u16 flags;
    u8 pad08[0x4C];
};
struct SceneProjectileConfig {
    u8 pad00[0x14];
    s32 attack14, attack18, attack1C, attack20, attack24;
    u8 pad28[0x14];
    u32 animation;
};
struct SceneProjectile {
    u32 age;
    u8 pad04[0xC];
    u32 scratch[2];
    u8 pad18[0x10];
    struct SceneProjectileConfig config;
    u8 pad68[0x18];
    u8 animation[0x1C];
    u8 sprite[0x2C];
    struct ProjectileCollider attack;
    struct ProjectileCollider body;
};
void Projectile_InitParameters(struct SceneProjectileConfig *);
s32 AnimData_Load(void *, u32);
void Sprite_InitUnlinked(void *, void *, s32);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void SceneProjectile_RespondToContact(void);
void SlotActor_ResetOnFlags(void);
static inline void ClearProjectileScratch(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
void SceneProjectile_InitSlot(void *pool, struct SceneProjectile *p, s32 index)
{
    struct ProjectileVector size, offset;
    struct SceneProjectileConfig *config = (void *)((u8 *)p + 0x28);
    struct ProjectileCollider *collider;
    Projectile_InitParameters(config);
    p->age = 0;
    ClearProjectileScratch(p->scratch);
    AnimData_Load(p->animation, config->animation);
    Sprite_InitUnlinked(p->sprite, p->animation, 1);
    collider = &p->attack;
    Collider_Init(collider, 0, 0x2001, 0, 16, (u8 *)p + 0x50, (u8 *)p + 0x58);
    Collider_SetCallback(collider, SceneProjectile_RespondToContact, p);
    Collider_SetAttackProperties(collider, config->attack1C, config->attack20, config->attack14,
                                 config->attack18, config->attack24);
    collider = &p->body;
    size.x = 12;
    size.y = 120;
    size.z = 12;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, SlotActor_ResetOnFlags, p);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
