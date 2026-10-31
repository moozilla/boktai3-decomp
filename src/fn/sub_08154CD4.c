#include "player_cue.h"
// confidence=low: inferred from player cue phases and animation consumers.
#include "player_cue_sequence.h"
#include "render.h"
// confidence=low: tentative cue role from callback phases.

void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_EnterStandUpdate(struct Player *);

void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_SetAnimation(struct Player *, s32, s32, s32);

static inline void PlayerCue_AdvanceRecoveryCounters(struct Player *p, u16 trailLimit,
                                                     u16 timerLimit)
{
    if ((p->actionEighteenDuration += 3) > trailLimit)
        p->actionEighteenDuration = trailLimit;
    if (++p->tetherFrame > timerLimit) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        PlayerCue_EnterStandUpdate(p);
        p->pendingAnimationTrigger = 1;
    }
}

// confidence=low: inferred from the side-facing cue callbacks and effects.
void PlayerCue_UpdateSpiralRecovery(struct Player *p)
{
    if (!p->tetherFrame) {
        p->actionEighteenDuration = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        PlayerCue_Coordinates(p)->previousPosition = PlayerCue_Coordinates(p)->position;
    }
    Player_SetAnimation(p, 0x9A, 0x40, 1);
    {
        u16 *target = &p->spriteResource;
        u32 resource;
        resource = 0x16F;
        *target = resource;
        p->actionVariantResource = (u32)(gPaletteBankData + (resource << 5));
    }
    {
        struct DisplaySequenceState *seq;
        seq = &PlayerCue_Graphics(p)->sequence;
        Anim_SetSequence(seq, PlayerCue_Graphics(p)->sideResource, 0, p->heading, p->side);
        {
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, seq);
        }
    }
    {
        u16 *target;
        u32 resource;
        target = &p->actionEighteenParameter;
        resource = 0x16F;
        *target = resource;
    }
    PlayerCue_AdvanceRecoveryCounters(p, 32, 31);
}
