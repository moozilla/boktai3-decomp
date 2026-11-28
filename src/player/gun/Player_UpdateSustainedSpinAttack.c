#include "global.h"
#include "player/player.h"
#include "scene/collision/contact_position.h"
#include "system/game_state/game_state.h"
extern const s16 gUnk_086149C4[];
u32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, u8);
void Player_StopActionSongOnExit(void *);
void Player_SetPendingCallback(void *, void (*)(void *));
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimation(void *, u32, u32, u32);
u8 *Player_GetSpecialAttackDescriptor(s32);
s32 ActorResponse_Apply(void *, s32);
void Sound_StopSong(u32);
void Sound_PlaySong(u32);
void Player_UpdateAttackFacing(void *);
void DirectionalSpriteEffect_PlaceOppositeAngle(void *, u32, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
u32 Player_GetAttackAttributeFlags(u32);
void Player_UpdateFacingSide(void *);
void ActorSequence_Begin(void *, u32, u32);
u32 Player_GetAttackEffectType(u32);
void AnimData_SelectPaletteBank(void *, u32);
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
// confidence=low: held input sustains the rotating attack collider while consuming energy.
void Player_UpdateSustainedSpinAttack(struct Player *p)
{
    struct ContactPosition pos;
    if (Player_ConsumeStateEntry(p)) {
        Player_SetActionPhase(p, 0);
        Player_SetPendingCallback(p, Player_StopActionSongOnExit);
    }
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x23), 64, 1)) {
                s32 cost;
                p->statModifierContext.chargeDuration = 30;
                Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind);
                if (p->statModifierContext.statBOverrideMode == 1)
                    cost = 0;
                else
                    cost = Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind)[2];
                cost = ActorResponse_Apply(p, cost);
                if (p->currentStatB < cost || p->statModifierContext.statBOverrideMode == 6) {
                    Player_SetActionPhase(p, 5);
                    Sound_StopSong(0xD5);
                } else {
                    Player_SetActionPhase(p, 1);
                    p->statModifierContext.contactTimer = (p->facing << 5) & 255;
                }
            }
            break;
        case 1:
            if (!p->tetherFrame)
                Sound_PlaySong(0x1F7);
            Player_UpdateAttackFacing(p);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            if (++p->tetherFrame > 6) {
                s32 enough;
                u8 *desc;
                s32 power;
                p->statModifierContext.chargeTier = 0;
                enough = 0;
                desc = Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind);
                if (!(p->tetherFrame & 1)) {
                    s32 cost;
                    if (p->statModifierContext.statBOverrideMode == 1)
                        cost = 0;
                    else
                        cost =
                            Player_GetSpecialAttackDescriptor(p->statModifierContext.actionKind)[2];
                    cost = ActorResponse_Apply(p, cost);
                    if (p->currentStatB < cost)
                        p->currentStatB = 0;
                    else {
                        p->currentStatB -= cost;
                        enough = 1;
                    }
                } else
                    enough = 1;
                power = gGameState->level + desc[1];
                if (!(p->inputKeys[0] & 2) || !enough) {
                    Sound_StopSong(0xCC);
                    Player_SetActionPhase(p, 6);
                } else {
                    s32 angle;
                    struct ContactCollider *collider;
                    if (p->tetherFrame == 8)
                        Sound_PlaySong(0xCC);
                    p->statModifierContext.contactTimer =
                        (p->statModifierContext.contactTimer + 16) & 255;
                    DirectionalSpriteEffect_PlaceOppositeAngle(
                        p, p->statModifierContext.contactTimer, &p->position);
                    pos = *(struct ContactPosition *)&p->position;
                    angle = (416 - p->statModifierContext.contactTimer) & 255;
                    pos.x += scale(170 * gUnk_086149C4[(angle + 64) & 255]);
                    pos.y += 230;
                    pos.z += scale(170 * gUnk_086149C4[angle]);
                    collider = &p->statModifierContext.attackCollider;
                    collider->unknown3C = power;
                    Collider_SetPosition(collider, &pos, 0);
                    collider->direction = angle;
                    Collider_Refresh(collider);
                    collider->reactionFlags =
                        Player_GetAttackAttributeFlags(p->statModifierContext.statBOverrideMode);
                    p->facing = ((p->statModifierContext.contactTimer + 15) & 255) >> 5;
                    Player_UpdateFacingSide(p);
                }
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 64, 1);
            }
            break;
        case 5:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x25), 64, 1);
            if (++p->tetherFrame > 4)
                Player_SetActionPhase(p, 6);
            break;
        case 6:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x24), 64, 1);
            if (++p->tetherFrame > 14) {
                Sound_StopSong(0xCC);
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
    AnimData_SelectPaletteBank(p->directionalEffect[0].resourceHeader,
                               p->statModifierContext.statBOverrideMode + 75);
}
