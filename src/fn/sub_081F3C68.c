#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: vector pointer lifetimes, collider argument reloads and loop scheduling.
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
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
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
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
#define H(o) (*(s16 *)((u8 *)p2 + (o)))
#define U(o) (*(u16 *)((u8 *)p2 + (o)))
#define B(o) (*(u8 *)((u8 *)p2 + (o)))
#define W(o) (*(u32 *)((u8 *)p2 + (o)))
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_StartAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
u8 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
static inline u32 randomA(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
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
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
struct Vec *Player_GetEffectivePosition(s32);
s32 Div(s32, s32);
void HomingProjectile_InitParams(void *);
void HomingProjectile_SetMotionParams(void *, s32, s32, u8, s32);
void HomingProjectile_SetPrimaryColliderParams(void *, s32, s32, s32);
void HomingProjectile_SetSecondaryColliderParams(void *, s32, s32, s32);
void HomingProjectile_Spawn(void *, struct Vec *);
void PlayerClone_InitConfig(void *);
void PlayerClone_SetLifetimeAndBehavior(void *, s32, s32, u8);
void PlayerClone_SetColliderExtents(void *, s32, s32, s32, s32, s32, s32);
s32 sub_08201160(void *, struct Vec *);
static inline u32 randomB(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
}
static inline void clampSpawn(struct Vec *spawn)
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
    u8 projectile[0x20];
    u8 clone[0x20];
    struct Vec delta;
    struct Vec spawn;
    struct Vec cloneDelta;
    struct Vec cloneSpawn;
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
            register s32 pattern asm("r10") = randomB();
            u8 i;
            pattern &= 3;
            for (i = 0; i < p2->projectileCount; i++) {
                register struct Vec *deltaPtr asm("r9") = &delta;
                register struct Vec *spawnPtr asm("r8") = &spawn;
                register u32 variant asm("r6") = randomA() & 1;
                spawn = *Player_GetEffectivePosition(0);
                polar(deltaPtr, (u8)angle, 512);
                sum(spawnPtr, spawnPtr, deltaPtr);
                HomingProjectile_InitParams(projectile);
                HomingProjectile_SetMotionParams(projectile, 60, 0, angle + 128, variant);
                HomingProjectile_SetPrimaryColliderParams(projectile, p2->primaryCollider[0],
                                                          p2->primaryCollider[1],
                                                          p2->primaryCollider[2]);
                if (variant == 0)
                    HomingProjectile_SetSecondaryColliderParams(
                        projectile, p2->secondaryColliderB[0], p2->secondaryColliderB[1],
                        p2->secondaryColliderB[2]);
                else
                    HomingProjectile_SetSecondaryColliderParams(
                        projectile, p2->secondaryColliderA[0], p2->secondaryColliderA[1],
                        p2->secondaryColliderA[2]);
                clampSpawn(&spawn);
                HomingProjectile_Spawn(projectile, &spawn);
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
                PlayerClone_InitConfig(clone);
                *((u32 *)(clone + 8)) = (randomB() & 15) + 16;
                PlayerClone_SetLifetimeAndBehavior(clone, 60, 5, angle + 128);
                PlayerClone_SetColliderExtents(clone, p2->cloneCollider[0], p2->cloneCollider[1],
                                               p2->cloneCollider[2], p2->cloneCollider[3],
                                               p2->cloneCollider[4], p2->cloneCollider[5]);
                clampSpawn(&cloneSpawn);
                sub_08201160(clone, &cloneSpawn);
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
