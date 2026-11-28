#include "scene/collision/contact_position.h"
#include "player/player.h"
#include "effects/particles/scene_particle_burst_api.h"
#include "global.h"
#include "player/player.h"
// confidence=low: charge phases and orbiting attack collider identify a spinning attack.
extern const s16 gUnk_086149C4[];
void Player_SetTetherMode(void *, u32, u32);
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimationIndex(void *, u32, u32);
u32 Player_UpdateFacingFromInputHistory(void *);

void Player_PlayLocalSong(u32, u32);
void Player_StopLocalSong(u32, u32);
u32 Player_GetConfiguredActionPower(void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

void LinkPlayer_SetScaledDirectionalVelocity(void *, u32);
void Player_UpdateActionFacingSide(void *);
void DirectionalEffect_PlaceSprite(void *, u32, void *);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
u32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, u8);
u32 Player_SetAnimation(void *, u32, u32, u32);
u32 Player_UpdateAttackFacing(void *);
u32 Player_GetAttackPower(void *);
void Player_SetDirectionalVelocity(void *, u32);
void Player_UpdateFacingSide(void *);
void DirectionalSpriteEffect_PlaceOppositeAngle(void *, u32, void *);
void Player_SpawnSelectedAttackProjectile(void *);
void ActorSequence_Begin(void *, u32, u32);
void Sound_PlaySong(u32);
// confidence=low: charge stages precede the selected weapon's orbiting spin collider.
void Player_UpdateSelectedSpinAttack(struct Player *p)
{
    if (Player_ConsumeStateEntry(p))
        Player_SetActionPhase(p, 0);
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40, 1)) {
                p->statModifierContext.chargeDuration = 30;
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            if (!p->tetherFrame)
                Sound_PlaySong(0x1F7);
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (++p->tetherFrame > 6) {
                p->statModifierContext.chargeTier = 0;
                if (p->currentStatB > 4) {
                    if (!(p->inputKeys[0] & 2)) {
                        Player_SetActionPhase(p, 5);
                        Player_SpawnSelectedAttackProjectile(p);
                    } else
                        Player_SetActionPhase(p, 2);
                }
            }
            break;
        case 2:
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (p->tetherFrame == 6)
                Sound_PlaySong(0x1F8);
            if (!(p->inputKeys[0] & 2)) {
                Player_SpawnSelectedAttackProjectile(p);
                Player_SetActionPhase(p, 5);
            } else if (++p->tetherFrame >= p->statModifierContext.chargeDuration) {
                p->statModifierContext.chargeTier = 1;
                p->actionEighteenParameter = 0x16F;
                p->actionEighteenDuration = 0x24;
                Sound_PlaySong(0x1F8);
                Player_SetActionPhase(p, 3);
            }
            break;
        case 3:
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (!(p->inputKeys[0] & 2)) {
                u32 power;

                Sound_PlaySong(0x2E9);
                p->statModifierContext.contactTimer = ((8 - p->facing) & 7) << 5;
                power = Player_GetAttackPower(p);
                Collider_SetAttackProperties(&p->statModifierContext.attackCollider, power, 0x20,
                                             0x10, 2, 20);
                Player_SpawnSelectedAttackProjectile(p);
                Player_SetActionPhase(p, 8);
            } else if (++p->tetherFrame >= p->statModifierContext.chargeDuration) {
                p->statModifierContext.chargeTier = 2;
                p->actionEighteenParameter = 0x16F;
                p->actionEighteenDuration = 0x24;
                Sound_PlaySong(0x1F9);
                Player_SetActionPhase(p, 4);
            }
            break;
        case 4:
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (!(p->inputKeys[0] & 2)) {
                u32 power;

                Sound_PlaySong(0x2E9);
                p->statModifierContext.contactTimer = ((8 - p->facing) & 7) << 5;
                power = Player_GetAttackPower(p);
                Collider_SetAttackProperties(&p->statModifierContext.attackCollider, power, 0x20,
                                             0x10, 2, 20);
                Player_SetActionPhase(p, 8);
            }
            break;
        case 5:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40, 1);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (++p->tetherFrame > 4)
                Player_SetActionPhase(p, 6);
            break;
        case 6: {
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 0x40, 1);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->statModifierContext.bufferedAttack) {
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else if (++p->tetherFrame > 14) {
                p->statModifierContext.bufferedAttack = 0;
                Player_SetActionPhase(p, 7);
            }
            break;
        }
        case 7:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 0x40, 1);
            if (++p->tetherFrame > 4)
                ActorSequence_Begin(p, 0, 0);
            break;
        case 8: {
            struct ContactPosition offset, size;
            struct ContactPosition pos;
            s32 angle;
            p->cameraInputProcessed = 0;
            if (Player_UpdateAttackFacing(p))
                Player_SetDirectionalVelocity(p, 6);
            if (!(p->tetherFrame & 3)) {
                offset.x = 0;
                offset.y = 0;
                offset.z = 0;
                size.x = 32;
                size.y = 0;
                size.z = 32;
                SceneParticleBurst_CreateWithPause((const struct ContactPosition *)&p->position,
                                                   &offset, &size, 1, 2, 4);
            }
            p->statModifierContext.contactTimer = (p->statModifierContext.contactTimer + 16) & 255;
            DirectionalSpriteEffect_PlaceOppositeAngle(p, p->statModifierContext.contactTimer,
                                                       &p->position);
            pos = *(struct ContactPosition *)&p->position;
            angle = (0x1A0 - p->statModifierContext.contactTimer) & 255;
            pos.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
            pos.y += 230;
            pos.z += scale(170 * gUnk_086149C4[angle]);
            Collider_SetPosition(&p->statModifierContext.attackCollider, &pos, 0);
            p->statModifierContext.attackCollider.direction = angle;
            Collider_Refresh(&p->statModifierContext.attackCollider);
            p->facing = ((p->statModifierContext.contactTimer + 15) & 255) >> 5;
            Player_UpdateFacingSide(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 0x40, 1);
            if (!p->tetherFrame)
                Sound_PlaySong(0x57C);
            if (++p->tetherFrame >= (p->statModifierContext.chargeTier << 5)) {
                Player_SetActionPhase(p, 6);
            }
            break;
        }
    }
    p->actionVariant = 9;
}
