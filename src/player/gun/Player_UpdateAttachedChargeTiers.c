#include "player/player.h"
#include "global.h"
#include "scene/collision/contact_actor.h"
#include "effects/bat_flock.h"
// confidence=low: role inferred from callers and callback phases.
// confidence=low: charge tiers and buffered input inferred from phase transitions.
extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);
u32 Player_ConsumeStateEntry(void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_SetActionPhase(void *, u8);
void Player_UpdateAttackFacing(void *);
void Sound_PlaySong(u32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 ActorResponse_Apply(void *, s32);
s32 Player_SpawnSelectedAttackProjectile(void *);
void Projectile_SetEntryDirection(s32, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 Player_GetAttackEffectType(u32);
void Player_OnChargedAttackExit(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
s32 ProjectileTask_GetKind7Value(s32);
void Player_UpdateAttachedEntryPosition(void *);
// confidence=low: attached task charge values choose tiers before recoil and buffered restart.
void Player_UpdateAttachedChargeTiers(struct Player *p)
{
    struct ContactPackedVector origin;
    struct ContactPackedVector spread;
    if (Player_ConsumeStateEntry(p)) {
        p->statModifierContext.contactTimer = 0;
        p->statModifierContext.attachedEntry = -1;
        Player_SetPendingCallback(p, Player_OnChargedAttackExit);
    }
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 64, 1)) {
                p->contactCounted = 0;
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            if (!p->tetherFrame)
                Sound_PlaySong(0x1F7);
            if (p->inputKeys[0] & 2) {
                Player_UpdateAttackFacing(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            }
            if (++p->tetherFrame > 6) {
                s32 cost = Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind)[2];
                p->statModifierContext.chargeTier = 0;
                p->contactMagnitude = 0;
                cost = ActorResponse_Apply(p, cost);
                if (p->currentStatB < cost) {
                    Sound_PlaySong(0xD5);
                    Player_SetActionPhase(p, 5);
                    break;
                }
                p->statModifierContext.chargeElapsed = 15;
                if (!Player_SpawnSelectedAttackProjectile(p))
                    Player_SetActionPhase(p, 5);
                else
                    Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            p->statModifierContext.chargeElapsed++;
            if (++p->tetherFrame == 6)
                Sound_PlaySong(0x1F8);
            if (!(p->inputKeys[0] & 2)) {
                Player_SetActionPhase(p, 5);
                Player_UpdateAttachedEntryPosition(p);
                break;
            }
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            if (ProjectileTask_GetKind7Value(p->statModifierContext.attachedEntry) > 1) {
                p->contactMagnitude = 30;
                p->actionEighteenParameter = 0x16E;
                p->actionEighteenDuration = 36;
                Player_SetActionPhase(p, 3);
            }
            Player_UpdateAttachedEntryPosition(p);
            break;
        case 3:
            p->statModifierContext.chargeElapsed++;
            if (!(p->inputKeys[0] & 2)) {
                Player_SetActionPhase(p, 5);
                Player_UpdateAttachedEntryPosition(p);
                break;
            }
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            if (ProjectileTask_GetKind7Value(p->statModifierContext.attachedEntry) > 2) {
                p->contactMagnitude = 60;
                p->actionEighteenParameter = 0x16E;
                p->actionEighteenDuration = 36;
                Sound_PlaySong(0x1F9);
                Player_SetActionPhase(p, 4);
            }
            Player_UpdateAttachedEntryPosition(p);
            break;
        case 4:
            p->statModifierContext.chargeElapsed++;
            if (!(p->inputKeys[0] & 2)) {
                Player_SetActionPhase(p, 5);
            } else {
                Player_UpdateAttackFacing(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            }
            Player_UpdateAttachedEntryPosition(p);
            break;
        case 5:
            if (!p->tetherFrame) {
                Player_UpdateAttachedEntryPosition(p);
                if (p->statModifierContext.attachedEntry >= 0 &&
                    !Player_SpawnSelectedAttackProjectile(p))
                    Player_SetActionPhase(p, 6);
                p->statModifierContext.contactTimer++;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 64, 1);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->contactMagnitude) {
                s32 ang, n;
                if (p->contactMagnitude > 5) {
                    origin.x = 0;
                    origin.y = 0;
                    origin.z = 0;
                    spread.x = 32;
                    spread.y = 32;
                    spread.z = 32;
                    SceneParticleBurst_CreateWhilePaused(&p->position, &origin, &spread, 1, 3, 4);
                }
                ang = ((p->facing + 5) & 7) << 5;
                n = p->contactMagnitude;
                p->velocityX = -scale(gUnk_086149C4[(ang + 64) & 255] * n);
                p->velocityZ = -scale(p->contactMagnitude * gUnk_086149C4[ang]);
                p->contactMagnitude = p->contactMagnitude * 3 >> 2;
            } else {
                if (++p->tetherFrame > 4)
                    Player_SetActionPhase(p, 6);
            }
            break;
        case 6:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->statModifierContext.bufferedAttack) {
                p->statModifierContext.attachedEntry = -1;
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else if (++p->tetherFrame > 14) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        case 7:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 64, 1);
            if (++p->tetherFrame > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
    p->actionVariant = Player_GetAttackEffectType(p->statModifierContext.statBOverrideMode);
}
