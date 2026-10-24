#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
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
s32 Math_Atan2(s32, s32);
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
// confidence=low: collision response queues Ratatosk damage and tracks repeated hits.
#define Ratatosk_OnDamageCollision sub_081F9BEC
void Ratatosk_OnDamageCollision(u8 *hit, u8 *response, struct Actor *owner)
{
    struct Actor *p2 = owner;
    struct Actor *p = owner;
    typeof(*((u16 *)(hit + 0x44))) t1;
    Collision_CalculateDamageResponse(hit, response);
    if (*((u16 *)(response + 0x44))) {
        {
            typeof(*((u16 *)(response + 0x3e))) *q1 = &(*((u16 *)(response + 0x3e)));
            u8 z1 = 0;
            *q1 = z1;
            response[0x43] = z1;
        }
    } else {
        u32 attack;
        u32 flags;
        p2->motion = *((u16 *)(response + 0x3e));
        *((u32 *)(((u8 *)p2) + 0xf08)) = *((u16 *)(hit + 10));
        t1 = *((u16 *)(hit + 0x44));
        *((u16 *)(((u8 *)p2) + 0x1cc)) = t1;
        if (p->flags & 0x80000)
            p2->motion = 0;
        if ((*((u16 *)(hit + 6))) & 0x100)
            p2->targetAngle = hit[0x42];
        else
            p2->targetAngle = Math_Atan2((*((s16 *)(response + 0x24))) - (*((s16 *)(hit + 0x24))),
                                         (*((s16 *)(response + 0x28))) - (*((s16 *)(hit + 0x28))));
        attack = *((u32 *)(hit + 0x34));
        flags = *((u32 *)(hit + 0x38));
        if (((((s16)p2->motion) > 0) && (p2->lastAttack == attack)) && (p2->damageWindow > 0)) {
            if (flags & 0x100)
                goto contact;
            if (((u8)(p->active - 9)) > 1) {
                if (attack & 16) {
                    if ((++p2->repeatHits) > 0) {
                        u8 mask = 0x200000;
                        p2->damageWindow = 0;
                        p->flags |= mask;
                    }
                } else if ((++p2->repeatHits) > 1) {
                    p2->damageWindow = 0;
                    {
                        u32 k1 = 0x400000;
                        p->flags |= k1;
                    }
                }
            }
        }
        if ((!(flags & 0x100)) && (((s16)p2->motion) > 0)) {
            p2->damageWindow = 120;
            p2->lastAttack = attack;
        }
    contact:
        *((u32 *)(((u8 *)p2) + 0xbc)) = attack;

        *((u32 *)(((u8 *)p2) + 0xc0)) = *((u32 *)(hit + 0x38));
        PlayerStats_AddContactHundredths(hit, response, *((u8 *)(((u8 *)p2) + 0xe2)));
        if ((*((u32 *)(hit + 0x38))) & 8)
            p2->motionFlag = 1;
        if ((*((u32 *)(hit + 0x38))) & 64) {
            *((u16 *)(((u8 *)p2) + 0x1cc)) = 38;
            p2->damageTimer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(((u8 *)p2) + 0x188, (s16)p2->motion, 0, ((u8 *)p2) + 0x194,
                                     ((u8 *)p2) + 0x19c, 1);
            if ((*((u32 *)(hit + 0x38))) & 64)
                return;
        }
        PlayerStats_AddContactHundredths(hit, response, *((s16 *)(((u8 *)p2) + 0xf0c)));
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F9BEC);
#endif
