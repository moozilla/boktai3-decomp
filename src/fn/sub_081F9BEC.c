#include "global.h"
#include "ratatosk.h"
#include "scene_indicator.h"

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
void Ratatosk_OnDamageCollision(u8 *hit, u8 *response, struct Ratatosk *owner)
{
    struct Ratatosk *p2 = owner;
    struct Ratatosk *p = owner;
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
        p2->frameDamage = ((struct HitResponse *)response)->motion;
        p2->incomingDamage = *((u16 *)(hit + 10));
        p2->hurtCollider.responseValue = *((u16 *)(hit + 0x44));
        actorMask = 0x80000;
        if (p->flags & actorMask)
            p2->frameDamage = 0;
        angleMask = 0x100;
        if ((*((u16 *)(hit + 6))) & angleMask)
            p2->targetAngle = hit[0x42];
        else
            p2->targetAngle = Math_Atan2((*((s16 *)(response + 0x24))) - (*((s16 *)(hit + 0x24))),
                                         (*((s16 *)(response + 0x28))) - (*((s16 *)(hit + 0x28))));
        motion = (s16)p2->frameDamage;
        attack = *((u32 *)(hit + 0x34));
        flags = *((u32 *)(hit + 0x38));
        if (motion > 0 && p2->lastAttack == attack && p2->damageWindow > 0) {
            if (flags & 0x100)
                goto contact;
            if ((u8)(p->actionPhase - 9) > 1) {
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
        if (!(flags & 0x100) && (s16)p2->frameDamage > 0) {
            p2->damageWindow = 120;
            p2->lastAttack = attack;
        }
    contact:
        p->contactAttack = attack;
        contactFlags = *((u32 *)(hit + 0x38));
        p->contactFlags = contactFlags;
        PlayerStats_AddContactHundredths(hit, response, p->healthRatio);
        contactMask = 8;
        if ((*((u32 *)(hit + 0x38))) & contactMask)
            p2->motionFlag = 1;
        if ((*((u32 *)(hit + 0x38))) & 64) {
            p2->hurtCollider.responseValue = 38;
            p2->damageTimer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(((u8 *)p2) + 0x188, (s16)p2->frameDamage, 0,
                                     ((u8 *)p2) + 0x194, ((u8 *)p2) + 0x19c, 1);
            if ((*((u32 *)(hit + 0x38))) & 64)
                return;
        }
        PlayerStats_AddContactHundredths(hit, response, p2->contactValue);
    }
}
