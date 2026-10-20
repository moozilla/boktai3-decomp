#include "global.h"
#define ScriptCombatActor_TryProjectileAttack sub_08201898

#define ScriptCombatActor_UpdateProjectileAttack sub_08201A78

#ifdef NONMATCHING
// fuzzy: 80.2%
struct Vec {
    s16 x, y, z, pad;
};
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
struct Vec *Player_GetEffectivePosition(u32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, s32, s32, s32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetVerticalMotion(void *, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void Projectile_SpawnFromConfig(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
u32 ScriptCombatActor_TryProjectileAttack(void *);
#define H(off) (*(s16 *)(p + (off)))
#define U(off) (*(u16 *)(p + (off)))
#define W(off) (*(u32 *)(p + (off)))
static inline s32 RandomValue(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)((u32)table + gUnk_03005308 * 2);
}
static inline s32 distanceSquared(struct Vec *a, struct Vec *b)
{
    s32 x = a->x - b->x, z = a->z - b->z;
    return x * x + z * z;
}
static inline void raisePosition(struct Vec *p)
{
    p->y += 128;
}
// Remaining differences: random-load lifetimes, stack-vector addressing, and registers.
u32 ScriptCombatActor_UpdateProjectileAttack(u8 *p)
{
    u8 config[0x5c];
    struct Vec position;
    s32 velocity;
    s32 distance;
    struct Vec *pos = &position;
    u32 angle;
    struct Vec *target;
    struct Vec *origin;
    if (Mod(H(0x184), 10) == 0 && H(0x184) > H(0x18e) - 30) {
        velocity = 0x1400;
        distance = distanceSquared(Player_GetEffectivePosition(0), (struct Vec *)(p + 0x40));
        origin = (struct Vec *)(p + 0x40);
        ProjectileConfig_Init(config);
        position = *(struct Vec *)(p + 0x40);
        raisePosition(pos);
        target = Player_GetEffectivePosition(0);
        angle = (u8)((s8)Math_Atan2(target->x - origin->x, target->z - origin->z) +
                     ((RandomValue(gUnk_0203B400) & 31) - 16));
        if (distance <= 0x15f8f)
            velocity = 0x1800;
        else if (distance > 0x40000)
            velocity = 0x1000;
        ProjectileConfig_SetAppearance(config, 0x7e7c, 0, -1);
        ProjectileConfig_SetCollision(config, 0, 0x200000, H(0x174), H(0x176), H(0x178));
        ProjectileConfig_SetMovement(config, 0, 120, angle, 12);
        ProjectileConfig_SetVerticalMotion(config, velocity, 0x18000);
        if (RandomValue(gUnk_0203B400) & 1)
            ProjectileConfig_SetAnimation(config, 0xa6cf, 0, 0);
        else
            ProjectileConfig_SetAnimation(config, 0xa6cf, 1, 0);
        Projectile_SpawnFromConfig(config, pos);
    }
    U(0x40) = U(0x1c) - 8 + (RandomValue(gUnk_0203B400) & 15);
    U(0x42) = U(0x1e);
    U(0x44) = U(0x20) - 8 + (RandomValue(gUnk_0203B400) & 15);
    if (H(0x184) > 0)
        U(0x184)--;
    else {
        Anim_RestartSequence(p + 0x160, *(void **)(p + 0x1cc), 0, 0, 0);
        *(u32(**)(void *))(p + 0x15c) = ScriptCombatActor_TryProjectileAttack;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08201A78);
#endif
