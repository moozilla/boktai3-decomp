#include "global.h"
#include "engine.h"

struct Vec {
    s16 x, y, z, pad;
};
struct ScriptCombatActor {
    u8 unknown0[0x1c];
    struct Vec origin;
    u8 unknown24[0x1c];
    struct Vec position;
    u8 unknown48[0x114];
    u32 (*update)(void *);
    u8 animation[0x10];
    s32 attackDistance;
    s16 attackA, attackB, attackC;
    u8 unknown17A[10];
    u16 timer;
    u8 unknown186[8];
    u16 duration;
    s16 enabled;
    u8 unknown192[4];
    u16 phase;
    u8 unknown198[0x34];
    void *sequence;
};
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
static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
}
static inline s32 distanceSquared(struct Vec *a, struct Vec *b)
{
    s32 x = a->x - b->x, z = a->z - b->z;
    return x * x + z * z;
}
static inline void CopyPosition(struct Vec *out, struct Vec *in)
{
    *out = *in;
}
static inline void raisePosition(struct Vec *p)
{
    p->y += 128;
}
u32 ScriptCombatActor_UpdateProjectileAttack(struct ScriptCombatActor *p)
{
    u8 config[0x5c];
    struct Vec position;
    s32 velocity;
    s32 distance;
    u32 angle;
    s8 aim;
    struct Vec *pos;
    struct Vec *target;
    struct Vec *origin;
    if (Mod((s16)p->timer, 10) == 0 && (s16)p->timer > (s16)p->duration - 30) {
        velocity = 0x1400;
        distance = distanceSquared(Player_GetEffectivePosition(0), &p->position);
        origin = &p->position;
        ProjectileConfig_Init(config);
        pos = &position;
        CopyPosition(pos, &p->position);
        raisePosition(pos);
        target = Player_GetEffectivePosition(0);
        aim = Math_Atan2(target->x - origin->x, target->z - origin->z);
        angle = (u8)(aim + ((RandomValue() & 31) - 16));
        if (distance <= 0x15f8f)
            velocity = 0x1800;
        else if (distance > 0x40000)
            velocity = 0x1000;
        ProjectileConfig_SetAppearance(config, 0x7e7c, 0, -1);
        ProjectileConfig_SetCollision(config, 0, 0x200000, p->attackA, p->attackB, p->attackC);
        ProjectileConfig_SetMovement(config, 0, 120, angle, 12);
        ProjectileConfig_SetVerticalMotion(config, velocity, 0x18000);
        if (RandomValue() & 1)
            ProjectileConfig_SetAnimation(config, 0xa6cf, 0, 0);
        else
            ProjectileConfig_SetAnimation(config, 0xa6cf, 1, 0);
        Projectile_SpawnFromConfig(config, pos);
    }
    {
        const u16 *table;
        u32 *cursor;
        u32 range;
        u32 mask;
        u32 index;
        table = (const u16 *)RANDOM_TABLE_ADDR;
        cursor = &gRandomIndex;
        index = *cursor;
        index++;
        mask = 1023;
        index &= mask;
        {
            u32 sample = table[*(&index)];
            s32 x = (u16)p->origin.x;
            x -= 8;
            range = 15;
            sample &= range;
            p->position.x = x + sample;
        }
        p->position.y = p->origin.y;
        index = (index + 1) & mask;
        *cursor = index;
        {
            u32 sample = table[index];
            s32 z = (u16)p->origin.z;
            z -= 8;
            sample &= range;
            p->position.z = z + sample;
        }
    }
    if ((s16)p->timer > 0)
        p->timer--;
    else {
        Anim_RestartSequence(p->animation, p->sequence, 0, 0, 0);
        p->update = ScriptCombatActor_TryProjectileAttack;
    }
    return 0;
}
