#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: register allocation, scheduling and random-source lifetimes.
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    s16 maxHealth;
    u8 pad98[4];
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
    s16 damageWindow, repeatHits;
    u8 padE7c[4];
    s16 vulnerability;
    u8 padE82[0x14];
    s16 healthPhase;
    u32 lastAttack;
    u8 padE9c[0x3a];
    s16 damageTimer;
    u8 padEd8[0x10];
    u32 savedPhase, savedTime, savedState, savedExtra;
};
#define H(o) (*(s16 *)((u8 *)p2 + (o)))
#define U(o) (*(u16 *)((u8 *)p2 + (o)))
#define B(o) (*(u8 *)((u8 *)p2 + (o)))
#define W(o) (*(u32 *)((u8 *)p2 + (o)))
void Sound_PlaySong(u32);
void GameState_SetEventFlag(s32);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const struct Vec gUnk_0824FDF0;
struct Vec *Ratatosk_GetRandomMovementPoint(void *);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Ratatosk_UpdateDefeatSequence(void);
void sub_081F5290(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Sound_PlaySongChecked(u32);
static inline u32 randomB(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
// confidence=low: consumes queued damage and transitions to recoil or defeat.
#define Ratatosk_ApplyQueuedDamage sub_081F6428
s32 Ratatosk_ApplyQueuedDamage(struct Actor *p)
{
    volatile struct Vec spawn;
    struct Vec velocity;
    struct Actor *p2 = p;
    s32 react = 0;
    s32 damage = (s16)p2->motion;
    u32 particles = p2->motionFlag;
    s32 oldPhase;
    p2->motion = react;
    p2->padE71 = 0;
    p2->motionFlag = 0;
    oldPhase = p2->healthPhase;
    if ((!oldPhase) &&
        (p->health < (((p->maxHealth >> 1) + (p->maxHealth >> 3)) + (p->maxHealth >> 4))))
        p2->healthPhase = 1;
    else if ((p2->healthPhase <= 1) &&
             (p->health < (((p->maxHealth >> 2) + (p->maxHealth >> 4)) + (p->maxHealth >> 5))))
        p2->healthPhase = 2;
    if (oldPhase != p2->healthPhase) {
        *((struct Vec *)(((u8 *)p2) + 0xe5c)) = *Ratatosk_GetRandomMovementPoint(p2);
        p2->savedState = 2;
    }
    if (damage > 0) {
        if ((p->flags & 16) || (p2->vulnerability > 0)) {
            react = 1;
            if (!Player_IsSlotInActionTwelveOrThirtyEight(0))
                damage *= 3;
            if (damage > 500)
                damage = (randomB() & 31) + 484;
            p2->vulnerability = 0;
        }
        p->health -= damage;
        p2->damageTimer = 8;
        if (p->health <= 0) {
            u32 zero = 0;
            p->health = zero;
            {
                u32 mask = 2;
                p->flags |= mask;
            }
            p->animationId = 15;
            p->update = Ratatosk_UpdateDefeatSequence;
            p->timer = zero;
            p->changed = 1;
            p->mode = 0;
            p->modeTime = zero;
            GameState_SetEventFlag(1);
            return 1;
        }
        if (damage > 0)
            DamageNumberDisplay_Show(((u8 *)p2) + 0x188, damage, 0, ((u8 *)p2) + 0x194,
                                     ((u8 *)p2) + 0x19c, 1);
        if (particles) {
            s32 i = 2;
            do {
                velocity = gUnk_0824FDF0;
                *((struct Vec *)(&spawn)) = p->position;
                spawn.x = (spawn.x - 64) + (randomB() & 127);
                spawn.y += (u8)randomB();
                spawn.z = (spawn.z - 64) + (randomB() & 127);
                do {
                    SparkParticle_Spawn(&spawn, 1, &velocity, p->angle + 128, 3, 8, 2, 2);
                } while (0);
            } while ((--i) >= 0);
        }
        if (react) {
            u32 one = 1;
            u32 zero = 0;
            p2->savedPhase = p2->phase;
            p2->savedTime = p->timer;
            p2->savedState = p->active;
            p2->savedExtra = *((u32 *)(((u8 *)p2) + 0x180));
            p->flags |= one;
            p->animationId = 18;
            p->update = sub_081F5290;
            p->timer = zero;
            p->changed = one;
            p->mode = zero;
            p->modeTime = zero;
            return 1;
        }
        Sound_PlaySongChecked(318);
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F6428);
#endif
