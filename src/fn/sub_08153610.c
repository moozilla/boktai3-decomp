#include "player_cue.h"
#include "player_cue_sequence.h"
#include "render.h"
#include "scene_vec.h"

// confidence=low: tentative player cue role; see callback body.

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdateHidden(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_RestartAnimationFromSetup(struct Player *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);
u32 Player_SetAnimation(struct Player *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, void *, s32, s32);

static inline void PlayerCue_AdvanceSpiralCounters(struct Player *p, u16 trailLimit, u16 timerLimit)
{
    if ((p->actionEighteenDuration += 3) > trailLimit)
        p->actionEighteenDuration = trailLimit;
    if (++p->tetherFrame > timerLimit) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        Player_UpdateFacingSide(p);
        PlayerCue_EnterStandUpdate(p);
        p->pendingAnimationTrigger = 1;
    }
}

void PlayerCue_UpdateSpiralMotion(struct Player *p)
{
    if (p->tetherFrame == 0) {
        if (p->side)
            p->facing = 7 - (p->heading * 2);
        else
            p->facing = (p->heading * 2) + 1;
        p->actionEighteenDuration = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    if ((p->tetherFrame & 7) < (7 - (p->tetherFrame >> 2))) {
        if (p->facing > 3) {
            p->heading = (7 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    } else {
        Player_AddControlFlags(p, 0x41);
        p->previousPosition = p->position;
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    {
        u16 *destination = &p->spriteResource;
        u32 resource = 0x16F;
        *destination = resource;
        p->actionVariantResource = (u32)(gPaletteBankData + (resource << 5));
    }
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, (void *)PlayerCue_Graphics(p)->setupResource,
                     0, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
    {
        u16 *destination = &p->actionEighteenParameter;
        u32 resource = 0x2DD;
        *destination = resource;
    }
    PlayerCue_AdvanceSpiralCounters(p, 32, 31);
}
