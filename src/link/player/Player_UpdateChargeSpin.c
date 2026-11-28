#include "scene/collision/contact_swarm.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"

void LinkPlayer_FlushPendingEffects(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 Player_GetLinkedSmoothedDirection(struct Player *);
s32 Player_GetAdjustedMovementInterval(struct Player *);
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *, s32);
void Player_UpdateActionFacingSide(struct Player *);
void Player_PlayLocalSong(u32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_UpdateChargeSpin(struct Player *p)
{
    if (p->tetherMode != 0xE) {
        Player_SetTetherMode(p, 0xE, 0);
        p->actionEighteenDuration = 0;
        LinkPlayer_FlushPendingEffects(p);
        Player_PlayLocalSong(p->slot, 0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 0x20, 0x38);
    }
    if (p->tetherFrame == 0x10)
        Player_PlayLocalSong(p->slot, 0x3A1);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_SetSecondaryActionFlags(p, 0x41);
        p->colliderPosition = p->position;
    }
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
    p->actionEighteenParameter = 0x16F;
    p->actionEighteenDuration += 3;
    if (p->actionEighteenDuration > 0x20)
        p->actionEighteenDuration = 0x20;
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    AnimData_SelectPaletteBank(p->visualObject, 0x16F);
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 4;
        Player_SetTetherMode(p, 0, 0);
    }
}
