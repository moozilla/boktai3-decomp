#include "global.h"
#include "engine.h"

struct Vec {
    u16 x, y, z, pad;
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
#define W(o) (*(u32 *)((u8 *)p2 + (o)))
void GameState_SetEventFlag(s32);
extern const u16 gUnk_0203B400[];
extern const struct Vec gUnk_0824FDF0;
struct Vec *Ratatosk_GetRandomMovementPoint(void *);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Ratatosk_UpdateDefeatSequence(void);
void sub_081F5290(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Sound_PlaySongChecked(u32);
static inline u32 randomTable(u32 table, u32 *index, u32 fullMask)
{
    *index = (*index + 1) & fullMask;
    return ((const u16 *)table)[*index];
}
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
static inline void setState(struct Actor *p, void (*update)(void), u8 state)
{
    p->animationId = state;
    p->update = update;
    p->timer = 0;
    p->changed = 1;
    p->mode = 0;
    p->modeTime = 0;
}
static inline u32 testFlags(struct Actor *p, u32 mask)
{
    return p->flags & mask;
}
// confidence=low: consumes queued damage and transitions to recoil or defeat.
#define Ratatosk_ApplyQueuedDamage sub_081F6428
s32 Ratatosk_ApplyQueuedDamage(struct Actor *p)
{
    struct {
        struct Vec spawn, velocity;
    } scratch;
    struct Actor *p2 = p;
    s32 react = 0;
    s32 damage = (s16)p2->motion;
    u32 particles = p2->motionFlag;
    s32 oldPhase;
    u32 changed;
    p2->motion = react;
    p2->padE71 = 0;
    p2->motionFlag = 0;
    oldPhase = p2->healthPhase;
    if (!p2->healthPhase &&
        p->health < ((p->maxHealth >> 1) + (p->maxHealth >> 3) + (p->maxHealth >> 4)))
        p2->healthPhase = 1;
    else if (p2->healthPhase <= 1 &&
             p->health < ((p->maxHealth >> 2) + (p->maxHealth >> 4) + (p->maxHealth >> 5)))
        p2->healthPhase = 2;
    if (oldPhase != p2->healthPhase) {
        *(struct Vec *)((u8 *)p2 + 0xe5c) = *Ratatosk_GetRandomMovementPoint(p2);
        p2->savedState = 2;
    }
    if (damage > 0) {
        if (testFlags(p, 16) || p2->vulnerability > 0) {
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
                changed = p->flags | mask;
                p->flags = changed;
            }
            {
                void (*update)(void);
                u32 animation;
                changed = 1;
                update = Ratatosk_UpdateDefeatSequence;
                animation = 15;
                p->animationId = animation;
                p->update = update;
                p->timer = zero;
                p->changed = changed;
                p->mode = 0;
                p->modeTime = zero;
            }
            GameState_SetEventFlag(1);
            return 1;
        }
        if (damage > 0)
            DamageNumberDisplay_Show((u8 *)p2 + 0x188, damage, 0, (u8 *)p2 + 0x194,
                                     (u8 *)p2 + 0x19c, 1);
        if (particles) {
            s32 i;
            u8 *angle;
            struct Vec *vel;
            struct Vec *pos;
            u32 *index;
            u32 fullMask, mask;
            vel = &scratch.velocity;
            angle = &p->angle;
            pos = &scratch.spawn;
            index = &gRandomIndex;
            fullMask = 1023;
            mask = 127;
            i = 2;
            do {
                u32 sample, x;
                u32 value;
                scratch.velocity = gUnk_0824FDF0;
                scratch.spawn = p->position;
                value = (u32)gUnk_0203B400;
                sample = randomTable(value, index, fullMask);
                x = pos->x - 64;
                sample &= mask;
                x += sample;
                pos->x = x;
                pos->y += (u8)randomTable(value, index, fullMask);
                sample = randomTable(value, index, fullMask);
                x = pos->z - 64;
                sample &= mask;
                x += sample;
                pos->z = x;
                value = *angle;
                value += 128;
                SparkParticle_Spawn(&scratch.spawn, 1, vel, value, 3, 8, 2, 2);
            } while (--i >= 0);
        }
        if (react) {
            u32 one;
            p2->savedPhase = p2->phase;
            p2->savedTime = p->timer;
            p2->savedState = p->active;
            p2->savedExtra = W(0x180);
            one = 1;
            p->flags |= one;
            setState(p, sub_081F5290, 18);
            return 1;
        }
        Sound_PlaySongChecked(318);
    }
    return 0;
}
