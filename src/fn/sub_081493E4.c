#include "player_cue.h"
#include "player.h"
#include "player_cue_sequence.h"
// confidence=low: provisional action interpretation from retained C and assembly.
// Remaining differences: instruction selection and register allocation.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
// confidence=low: holds the auxiliary pose until the movement latch clears.
void Player_UpdateFacingSide(struct Player *);
void Player_UpdateMovementPose(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    }
    p->statusFlags &= ~1;
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if (!p->movementModifierActive) {
        Player_UpdateFacingSide(p);
        ActorSequence_Begin(p, 0, 0);
    }
}
