#include "monsters/muspell_head.h"
#include "engine/engine.h"
#include "engine/math/monster_particle_random.h"

struct ContactPosition *Player_GetEffectivePosition(u32);
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
u32 MuspellHead_TryProjectileAttack(void *);

static inline s32 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
}

static inline s32 distanceSquared(struct ContactPosition *a, struct ContactPosition *b)
{
    s32 x = a->x - b->x, z = a->z - b->z;
    return x * x + z * z;
}
u32 MuspellHead_UpdateProjectileAttack(struct MuspellHead *head)
{
    u8 config[0x5c];
    struct ContactPosition position;
    u32 sample;
    s32 x;
    s32 z;
    s32 velocity;
    s32 distance;
    u32 angle;
    s8 aim;
    struct ContactPosition *pos;
    struct ContactPosition *target;
    struct ContactPosition *origin;
    if (Mod((s16)head->timer, 10) == 0 && (s16)head->timer > (s16)head->duration - 30) {
        velocity = 0x1400;
        distance = distanceSquared(Player_GetEffectivePosition(0), &head->sprite.position);
        origin = &head->sprite.position;
        ProjectileConfig_Init(config);
        pos = &position;
        *pos = head->sprite.position;
        pos->y += 128;
        target = Player_GetEffectivePosition(0);
        aim = Math_Atan2(target->x - origin->x, target->z - origin->z);
        angle = (u8)(aim + ((RandomValue() & 31) - 16));
        if (distance <= 0x15f8f)
            velocity = 0x1800;
        else if (distance > 0x40000)
            velocity = 0x1000;
        ProjectileConfig_SetAppearance(config, 0x7e7c, 0, -1);
        ProjectileConfig_SetCollision(config, 0, 0x200000, head->attackA, head->attackB,
                                      head->attackC);
        ProjectileConfig_SetMovement(config, 0, 120, angle, 12);
        ProjectileConfig_SetVerticalMotion(config, velocity, 0x18000);
        if (RandomValue() & 1)
            ProjectileConfig_SetAnimation(config, 0xa6cf, 0, 0);
        else
            ProjectileConfig_SetAnimation(config, 0xa6cf, 1, 0);
        Projectile_SpawnFromConfig(config, pos);
    }
    sample = ParticleRandom_ReadDirectSample(gRandomTable);
    x = (u16)head->origin.x - 8;
    head->sprite.position.x = x + (sample & 15);
    head->sprite.position.y = head->origin.y;
    sample = ParticleRandom_ReadDirectSample(gRandomTable);
    z = (u16)head->origin.z - 8;
    head->sprite.position.z = z + (sample & 15);
    if ((s16)head->timer > 0)
        head->timer--;
    else {
        Anim_RestartSequence(&head->playback, head->sequence, 0, 0, 0);
        head->action = (void (*)(void))MuspellHead_TryProjectileAttack;
    }
    return 0;
}
