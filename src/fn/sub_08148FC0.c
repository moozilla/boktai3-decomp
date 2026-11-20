#include "player_cue.h"
#include "game_state_flags.h"
#include "game_state.h"
#include "contact_swarm.h"
#include "player_cue_sequence.h"
// confidence=low: walking animation alternates footstep sounds and positional effects.

u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row, u32 flags);
s32 RepeatedEffect_RequestNearby(void *);
void Sound_PlaySong(u32 song);
void Player_UpdateFacingSide(void *);
u32 Player_ConsumeStateEntry(struct Player *p);
u8 Player_GetSmoothedDirection(void *);
s32 Player_GetWalkSpeed(void *p);
void Player_SetDirectionalVelocity(void *, s32);
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);

void Player_UpdateWalkFootsteps(struct Player *p)
{
    struct DisplaySequenceState *seq;

    if (Player_ConsumeStateEntry(p))
        p->footstepSide = 0;
    p->statusFlags &= ~1;
    p->facing = Player_GetSmoothedDirection(p);
    Player_SetDirectionalVelocity(p, Player_GetWalkSpeed(p));
    Player_UpdateFacingSide(p);
    seq = &p->cueCollider.sequence;
    Anim_SetSequence(seq, p->cueCollider.sideResource, 1, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    {
        if (p->cueCollider.sequence.index == 2) {
            u32 z = p->cueCollider.sequence.elapsed;
            if (z == 0) {
                if (RepeatedEffect_RequestNearby(&p->position)) {
                    if (p->footstepSide) {
                        p->footstepSide = z;
                        Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, z, 0x163);
                    } else {
                        p->footstepSide = 1;
                        Player_PlayPositionEffect(p, &p->position, 1, 1, &p->position, 0, 0x162);
                    }
                } else {
                    if (p->footstepSide) {
                        p->footstepSide = 0;
                        Sound_PlaySong(0xc7);
                    } else {
                        p->footstepSide = 1;
                        Sound_PlaySong(0xc8);
                    }
                    if (GameState_TestWorldFlags(1))
                        Player_PlayPositionEffect(p, &p->position, 1, 2, 0, 0, 0);
                }
            }
        }
    }
}
