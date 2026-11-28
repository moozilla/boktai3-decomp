#include "player/cues/player_cue.h"
#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
// confidence=low: holds the odd-direction pose until movement animation ends.
void Player_UpdateFacingSide(struct Player *);
void Player_UpdateOddDirectionPose(struct Player *p)
{
    Player_ConsumeStateEntry(p);
    if ((s16)p->directionHistory[0] >= 0) {
        u8 *facing = &p->facing;
        *facing = (p->directionHistory[0] & 254) + 1;
        if (*facing > 4) {
            s32 heading = (8 - *facing) >> 1;
            p->heading = heading;
            p->side = 1;
        } else {
            u32 heading = *facing >> 1;
            u8 *slot = &p->heading;
            *slot = heading;
            p->side = 0;
        }
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if ((s8)p->movementAnimationState < 0) {
        Player_UpdateFacingSide(p);
        ActorSequence_Begin(p, 0, 0);
    }
}
