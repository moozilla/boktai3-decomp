#include "global.h"

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
    u8 padB4[8];
    u32 contactAttack, contactFlags;
    u8 padC4[8];
    u8 requestedAngle;
    u8 padCD[0x15];
    u8 contactHundredths;
    u8 padE3[0x1d];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x4c];
    u16 hitEffect;
    u8 pad1CE[0x15e];
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
    u8 padEF8[0x10];
    u32 incomingDamage;
    s16 contactValue;
};
struct HitResponse {
    u8 pad0[0x3e];
    u16 motion;
    u8 pad40[3];
    u8 state;
    u16 effect;
};
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
    Collision_CalculateDamageResponse(hit, response);
    if (((struct HitResponse *)response)->effect) {
        ((struct HitResponse *)response)->motion = 0;
        ((struct HitResponse *)response)->state = 0;
    } else {
        u32 attack;
        u32 flags;
        u32 contactFlags;
        u32 actorMask, angleMask, repeatMask, contactMask;
        s32 motion;
        p2->motion = ((struct HitResponse *)response)->motion;
        p2->incomingDamage = *((u16 *)(hit + 10));
        p2->hitEffect = *((u16 *)(hit + 0x44));
        actorMask = 0x80000;
        if (p->flags & actorMask)
            p2->motion = 0;
        angleMask = 0x100;
        if ((*((u16 *)(hit + 6))) & angleMask)
            p2->targetAngle = hit[0x42];
        else
            p2->targetAngle = Math_Atan2((*((s16 *)(response + 0x24))) - (*((s16 *)(hit + 0x24))),
                                         (*((s16 *)(response + 0x28))) - (*((s16 *)(hit + 0x28))));
        motion = (s16)p2->motion;
        attack = *((u32 *)(hit + 0x34));
        flags = *((u32 *)(hit + 0x38));
        if (motion > 0 && p2->lastAttack == attack && p2->damageWindow > 0) {
            if (flags & 0x100)
                goto contact;
            if ((u8)(p->active - 9) > 1) {
                if (attack & 16) {
                    if (++p2->repeatHits > 0) {
                        p2->damageWindow = 0;
                        repeatMask = 0x200000;
                        p->flags |= repeatMask;
                    }
                } else if (++p2->repeatHits > 1) {
                    p2->damageWindow = 0;
                    repeatMask = 0x400000;
                    p->flags |= repeatMask;
                }
            }
        }
        if (!(flags & 0x100) && (s16)p2->motion > 0) {
            p2->damageWindow = 120;
            p2->lastAttack = attack;
        }
    contact:
        p->contactAttack = attack;
        contactFlags = *((u32 *)(hit + 0x38));
        p->contactFlags = contactFlags;
        PlayerStats_AddContactHundredths(hit, response, p->contactHundredths);
        contactMask = 8;
        if ((*((u32 *)(hit + 0x38))) & contactMask)
            p2->motionFlag = 1;
        if ((*((u32 *)(hit + 0x38))) & 64) {
            p2->hitEffect = 38;
            p2->damageTimer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(((u8 *)p2) + 0x188, (s16)p2->motion, 0, ((u8 *)p2) + 0x194,
                                     ((u8 *)p2) + 0x19c, 1);
            if ((*((u32 *)(hit + 0x38))) & 64)
                return;
        }
        PlayerStats_AddContactHundredths(hit, response, p2->contactValue);
    }
}
