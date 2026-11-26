#include "player/player_cue.h"
#include "player/player.h"
#include "player/player_cue/player_cue_sequence.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
// confidence=low: uses a stored direction sample until recovery animation completes.
void Player_UpdateDirectionRecovery(struct Player *p)
{
    if (Player_ConsumeStateEntry(p))
        ActorSequence_Begin(p, 23, 0);
    if ((s16)p->directionHistory[0] >= 0) {
        u8 *facing = &p->facing;
        p->facing = (p->directionHistory[0] & 254) + 1;
        if (*facing > 4) {
            p->heading = (8 - *facing) >> 1;
            p->side = 1;
        } else {
            p->heading = *facing >> 1;
            p->side = 0;
        }
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.setupResource, 1, p->heading,
                     p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if ((s8)p->movementAnimationState < 0)
        ActorSequence_Begin(p, 0, 0);
}
