#include "contact_position.h"
#include "ram.h"
#include "engine_globals.h"

#ifdef NONMATCHING
// fuzzy: 71.4%
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
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct ContactPosition position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct ContactPosition anchor;
    u8 pad374[0x58];
    struct ContactPosition home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
    u8 padE78[0x3c];
    s16 primaryCollider[3];
    s16 secondaryColliderA[3];
    s16 secondaryColliderB[3];
    s16 cloneCollider[6];
    u8 padEd2[8];
    s16 projectileCount, cloneCount;
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
static inline void facing(struct Actor *p, s32 angle)
{
    u32 v;
    s32 shift;
    p->angle = angle;
    shift = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shift;
    p->facing = v;
    shift = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shift;
    p->facing8 = v;
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct ContactPosition *d, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    d->x = b->x + a->x;
    d->y = a->y + b->y;
    d->z = b->z + a->z;
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
s32 sub_08201160(void *, struct ContactPosition *);
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
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
// confidence=low: surrounds the player with homing projectiles or clone actors.
#define Ratatosk_UpdateRadialSummonAttack sub_081F3C68
void Ratatosk_UpdateRadialSummonAttack(struct Actor *p, s32 tick)
{
    struct SummonConfig projectile, clone;
    struct ContactPosition delta;
    struct ContactPosition spawn;
    struct ContactPosition cloneDelta;
    struct ContactPosition cloneSpawn;
    struct Actor *p2 = p;
    facing(p, p->requestedAngle);
    if (p->changed) {
        p->changed = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 7, 2, 3, 4);
        if (p->active == 3)
            Sound_PlaySong(0x55a);
        else
            Sound_PlaySong(0x557);
    }
    if (tick == 13)
        Sound_PlaySong(0x565);
    if (tick > 56) {
        if (p->active == 3) {
            u8 angle = (u8)randomA();
            u16 pattern = randomB();
            s32 i;
            pattern &= 3;
            for (i = 0; i < p2->projectileCount; i++) {
                struct ContactPosition *deltaPtr = &delta;
                struct ContactPosition *spawnPtr = &spawn;
                u32 variant = randomA() & 1;
                *spawnPtr = *Player_GetEffectivePosition(0);
                polar(deltaPtr, (u8)angle, 512);
                sum(spawnPtr, spawnPtr, deltaPtr);
                HomingProjectile_InitParams(&projectile);
                HomingProjectile_SetMotionParams(&projectile, 60, 0, angle + 128, variant);
                HomingProjectile_SetPrimaryColliderParams(&projectile, p2->primaryCollider[0],
                                                          p2->primaryCollider[1],
                                                          p2->primaryCollider[2]);
                if (variant == 0)
                    HomingProjectile_SetSecondaryColliderParams(
                        &projectile, p2->secondaryColliderB[0], p2->secondaryColliderB[1],
                        p2->secondaryColliderB[2]);
                else
                    HomingProjectile_SetSecondaryColliderParams(
                        &projectile, p2->secondaryColliderA[0], p2->secondaryColliderA[1],
                        p2->secondaryColliderA[2]);
                clampSpawn(spawnPtr);
                HomingProjectile_Spawn(&projectile, spawnPtr);
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

                angle = (u8)angle;
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
                PlayerClone_SetColliderExtents(&clone, p2->cloneCollider[0], p2->cloneCollider[1],
                                               p2->cloneCollider[2], p2->cloneCollider[3],
                                               p2->cloneCollider[4], p2->cloneCollider[5]);
                clampSpawn(&cloneSpawn);
                sub_08201160(&clone, &cloneSpawn);
                angle += Div(256, p2->cloneCount);
            }
        }
        Sound_PlaySong(0x566);
        p->finished = 1;
    }
    if (!Callback_InvokeActor(p, p->callback)) {
        u32 mask = 0x80000;
        p->flags |= mask;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F3C68);
#endif
