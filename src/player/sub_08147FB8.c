#include "player/player_cue.h"
#include "engine/shared/player_status_flags.h"
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
// confidence=low: timed palette cue follows the spiral release.
void Player_UpdateSpiralAfterglow(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    Player_AddStatusFlags(p, 0x100000);
    if (p->tetherFrame == 16)
        Sound_PlaySong(0x3A1);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        p->colliderPosition = p->position;
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
    p->actionEighteenParameter = 0x16F;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    AnimData_SelectPaletteBank(p->visualObject, 0x16F);
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 4;
        ActorSequence_Begin(p, 0, 0);
    }
}
