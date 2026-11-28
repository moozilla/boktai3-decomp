#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "ui/hud/scene_indicator.h"

void Sound_PlaySong(u32);
s32 Math_Atan2(s32, s32);
void Collision_CalculateDamageResponse(struct ContactCollider *, struct ContactCollider *);
void PlayerStats_AddContactHundredths(struct ContactCollider *, struct ContactCollider *, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
// confidence=low: collision response queues Ratatosk damage and tracks repeated hits.
void Ratatosk_OnDamageCollision(struct ContactCollider *hit, struct ContactCollider *response,
                                struct Ratatosk *owner)
{
    struct Ratatosk *p = owner;
    Collision_CalculateDamageResponse(hit, response);
    if (response->responseValue) {
        response->cost = 0;
        response->costFlags = 0;
    } else {
        u32 attack;
        u32 flags;
        s32 motion;
        u32 contactFlags;
        owner->frameDamage = response->cost;
        owner->incomingDamage = hit->contactFlags;
        owner->hurtCollider.responseValue = hit->responseValue;
        if (Ratatosk_TestFlags(p, 0x80000))
            owner->frameDamage = 0;
        if (ContactCollider_TestFlags(hit, 0x100))
            owner->targetAngle = hit->direction;
        else
            owner->targetAngle = Math_Atan2(response->x - hit->x, response->z - hit->z);
        motion = owner->frameDamage;
        attack = hit->statRewardFlags;
        flags = hit->reactionFlags;
        if (motion > 0 && owner->lastAttack == attack && owner->damageWindow > 0) {
            if (flags & 0x100)
                goto contact;
            if ((u8)(p->actionPhase - 9) > 1) {
                if (attack & 16) {
                    if (++owner->repeatHits > 0) {
                        owner->damageWindow = 0;
                        Ratatosk_SetFlags(p, 0x200000);
                    }
                } else if (++owner->repeatHits > 1) {
                    owner->damageWindow = 0;
                    Ratatosk_SetFlags(p, 0x400000);
                }
            }
        }
        if (!(flags & 0x100) && owner->frameDamage > 0) {
            owner->damageWindow = 120;
            owner->lastAttack = attack;
        }
    contact:
        p->contactAttack = attack;
        contactFlags = hit->reactionFlags;
        p->contactFlags = contactFlags;
        PlayerStats_AddContactHundredths(hit, response, p->healthRatio);
        if (ContactCollider_TestReactionFlags(hit, 8))
            owner->motionFlag = 1;
        if (hit->reactionFlags & 64) {
            owner->hurtCollider.responseValue = 38;
            owner->damageTimer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&owner->hurtCollider, owner->frameDamage, 0,
                                     &owner->hurtCollider.position, &owner->hurtCollider.offset, 1);
            if (hit->reactionFlags & 64)
                return;
        }
        PlayerStats_AddContactHundredths(hit, response, owner->contactValue);
    }
}
