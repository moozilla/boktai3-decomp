#include "contact_swarm.h"
#include "player_cue_sequence.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: walking animation alternates footstep sounds and positional effects.

u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row, u32 flags);
s32 RepeatedEffect_RequestNearby(void *);
void Sound_PlaySong(u32 song);
void Player_UpdateFacingSide(void *);
u32 Player_ConsumeStateEntry(u8 *p);
u8 Player_GetSmoothedDirection(void *);
s32 Player_GetWalkSpeed(void *p);
void Player_SetDirectionalVelocity(void *, s32);
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);

static inline u8 global_bit(void)
{
    u32 mask = 1;
    if (*(u32 *)((u8 *)gGameState + 0x868) & mask)
        return TRUE;
    return FALSE;
}

void Player_UpdateWalkFootsteps(u8 *p)
{
    struct DisplaySequenceState *seq;

    if (Player_ConsumeStateEntry(p))
        p[0x46a] = 0;
    *(u32 *)(p + 0x20) &= ~1;
    p[0x3a4] = Player_GetSmoothedDirection(p);
    Player_SetDirectionalVelocity(p, Player_GetWalkSpeed(p));
    Player_UpdateFacingSide(p);
    seq = (struct DisplaySequenceState *)(p + 0x3fc);
    Anim_SetSequence(seq, *(void **)(p + 0x410), 1, p[0x3a2], p[0x3a3]);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)(p + 0x14C),
                           (struct DisplaySequenceState *)(p + 0x3FC));
    {
        u32 offset = 0x404;
        u16 *frame;
        frame = (u16 *)(p + offset);
        if (*frame == 2) {
            u32 z = *(u16 *)(p + 0x40a);
            u8 *fx;
            if (z == 0) {
                fx = p + 0x30;
                asm("" : "+r"(fx));
                if (RepeatedEffect_RequestNearby(fx)) {
                    if (p[0x46a]) {
                        p[0x46a] = z;
                        Player_PlayPositionEffect(p, fx, 1, 1, fx, z, 0x163);
                    } else {
                        p[0x46a] = 1;
                        Player_PlayPositionEffect(p, fx, 1, 1, fx, 0, 0x162);
                    }
                } else {
                    if (p[0x46a]) {
                        p[0x46a] = 0;
                        Sound_PlaySong(0xc7);
                    } else {
                        p[0x46a] = 1;
                        Sound_PlaySong(0xc8);
                    }
                    if (global_bit())
                        Player_PlayPositionEffect(p, fx, 1, 2, 0, 0, 0);
                }
            }
        }
    }
}
