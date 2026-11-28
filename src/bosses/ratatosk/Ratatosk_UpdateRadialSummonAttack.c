#include "bosses/ratatosk/ratatosk.h"
#include "scene/collision/contact_position.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/math/monster_particle_random.h"

struct PositionWords {
    u32 xy, zPad;
};
struct SummonConfig {
    u16 flags, lifetime;
    u8 unknown4[2], angle, variant;
    u32 behaviorValue;
    void *callback;
    s16 primaryCollider[3];
    u16 unknown16;
    s16 secondaryCollider[3];
    u16 unknown1E;
};
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
static inline u32 randomA(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct ContactPosition *d, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}

struct ContactPosition *Player_GetEffectivePosition(s32);
s32 Div(s32, s32);
void HomingProjectile_InitParams(void *);
void HomingProjectile_SetMotionParams(void *, s32, s32, u8, s32);
void HomingProjectile_SetPrimaryColliderParams(void *, s32, s32, s32);
void HomingProjectile_SetSecondaryColliderParams(void *, s32, s32, s32);
void HomingProjectile_Spawn(void *, struct ContactPosition *);
void PlayerClone_InitConfig(void *);
void PlayerClone_SetLifetimeAndBehavior(void *, s32, s32, u8);
void PlayerClone_SetColliderExtents(void *, s32, s32, s32, s32, s32, s32);
s32 PlayerClone_Spawn(void *, struct ContactPosition *);
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}
static inline struct ContactPosition *ClampSpawnX(struct ContactPosition *point)
{
    if (point->x <= 1059)
        point->x = 1060;
    if (point->x > 3290) {
        point->x = 3290;
        return point;
    }
    return point;
}
static inline s32 ReadPositionZ(struct ContactPosition *const position)
{
    return position->z;
}
static inline void ClampProjectileSpawn(struct ContactPosition *spawn)
{
    struct ContactPosition *point = ClampSpawnX(spawn);
    if (ReadPositionZ(point) <= 1319)
        point->z = 1320;
    if (point->z > 3030)
        point->z = 3030;
}
static inline void clampSpawn(struct ContactPosition *spawn)
{
    if (spawn->x <= 1059)
        spawn->x = 1060;
    if (spawn->x > 3290)
        spawn->x = 3290;
    if (spawn->z <= 1319)
        spawn->z = 1320;
    if (spawn->z > 3030)
        spawn->z = 3030;
}
static inline void Ratatosk_CopyPositionWords(void *dest, const void *source)
{
    *(struct PositionWords *)dest = *(const struct PositionWords *)source;
}

// confidence=low: surrounds the player with homing projectiles or clone actors.
void Ratatosk_UpdateRadialSummonAttack(struct Ratatosk *p, s32 tick)
{
    struct SummonConfig projectile, clone;
    struct ContactPosition positions[2];
    struct ContactPosition cloneDelta;
    struct ContactPosition cloneSpawn;
    struct Ratatosk *p2 = p;
    Ratatosk_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 7, 2, 3, 4);
        if (p->actionPhase == 3)
            Sound_PlaySong(0x55a);
        else
            Sound_PlaySong(0x557);
    }
    if (tick == 13)
        Sound_PlaySong(0x565);
    if (tick > 56) {
        if (p->actionPhase == 3) {
            u8 angle = (u8)randomA();
            u16 pattern = randomB();
            s32 i;
            pattern &= 3;
            for (i = 0; i < p2->projectileCount; i++) {
                struct ContactPosition *deltaPtr = &positions[0];
                struct ContactPosition *spawnPtr;
                u32 variant;
                gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
                PARTICLE_RANDOM_READ_SAMPLE(variant, gUnk_086141BC, gEffectRandomTableIndex);
                variant &= 1;
                spawnPtr = deltaPtr + 1;
                Ratatosk_CopyPositionWords(&positions[1], Player_GetEffectivePosition(0));
                polar(deltaPtr, angle, 512);
                sum(spawnPtr, spawnPtr, deltaPtr);
                HomingProjectile_InitParams(&projectile);
                HomingProjectile_SetMotionParams(&projectile, 60, 0, angle + 128, variant);
                HomingProjectile_SetPrimaryColliderParams(&projectile, p2->projectileAttack[0],
                                                          p2->projectileAttack[1],
                                                          p2->projectileAttack[2]);
                {
                    struct ContactPosition *spawnPoint = &positions[1];
                    if (variant == 0)
                        HomingProjectile_SetSecondaryColliderParams(
                            &projectile, p2->alternateAttack[0], p2->alternateAttack[1],
                            p2->alternateAttack[2]);
                    else
                        HomingProjectile_SetSecondaryColliderParams(
                            &projectile, p2->secondaryAttack[0], p2->secondaryAttack[1],
                            p2->secondaryAttack[2]);
                    ClampProjectileSpawn(spawnPoint);
                    HomingProjectile_Spawn(&projectile, spawnPoint);
                }
                switch (pattern) {
                    case 0:
                        angle += Div(256, p2->projectileCount);
                        break;

                    case 1:
                        angle += 32;
                        break;

                    case 2:
                        if ((i == 0) || (i == 2))
                            angle += 32;
                        else
                            angle += 96;
                        break;

                    case 3:
                        if (i != 2)
                            angle += 32;
                        else
                            angle += 96;
                        break;
                }
            }

        } else {
            u8 angle = randomA();
            s32 i;
            for (i = 0; i < p2->cloneCount; i++) {
                cloneSpawn = *Player_GetEffectivePosition(0);
                polar(&cloneDelta, angle, 512);
                sum(&cloneSpawn, &cloneSpawn, &cloneDelta);
                PlayerClone_InitConfig(&clone);
                clone.behaviorValue = (randomB() & 15) + 16;
                PlayerClone_SetLifetimeAndBehavior(&clone, 60, 5, angle + 128);
                PlayerClone_SetColliderExtents(&clone, p2->cloneAttack[0], p2->cloneAttack[1],
                                               p2->cloneAttack[2], p2->cloneAttack[3],
                                               p2->cloneAttack[4], p2->cloneAttack[5]);
                clampSpawn(&cloneSpawn);
                PlayerClone_Spawn(&clone, &cloneSpawn);
                angle += Div(256, p2->cloneCount);
            }
        }
        Sound_PlaySong(0x566);
        p->actionFinished = 1;
    }
    if (!Callback_InvokeActor(p, p->actionTryUpdate)) {
        Ratatosk_SetFlags(p, 0x80000);
    }
}
