#include "global.h"
#include "screamer_playback.h"
#include "player_cue_sequence.h"
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
static inline u32 Screamer_StepScreamLoop(struct ScreamerState *owner, u32 initialBit)
{
    return PlayerCue_StepSequenceEndedWithFirstFlipMask(&owner->sprite, &owner->sequence,
                                                        initialBit);
}

// The recovery branch loads its duration mask through r3.
static inline u32 Screamer_StepRecovery(struct ScreamerState *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}

s32 Screamer_UpdateScream(struct ScreamerState *p)
{
    s32 phase;
    p->sprite.flags &= ~1;
    phase = p->f1D6;
    switch (phase) {
        case 0:
            Screamer_StepAnimation(p);
            if (({
                    u32 off = 0x1ca;
                    s16 *field;
                    field = (void *)((u8 *)p + off);
                    *field;
                }) > ({
                    u32 off = 0x1d4;
                    s16 *field;
                    field = (void *)((u8 *)p + off);
                    *field;
                })) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1B, (u8)p->f1D8, 0);
                Anim_SetDurationScale(&p->sequence, 0x20);
                p->f1D6 = 1;
                Sound_PlaySong(0x51A);
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
        case 1:
            if (Screamer_StepScreamLoop(p, phase)) {
                {
                    u32 countOffset = 0x1D2;
                    u16 *count;
                    u32 next;
                    u32 limitOffset;
                    s16 *limit;
                    count = (void *)((u8 *)p + countOffset);
                    next = *count + 1;
                    *count = next;
                    limitOffset = 0x1D0;
                    limit = (void *)((u8 *)p + limitOffset);
                    if ((s16)next >= ({
                            s32 value;
                            value = *limit;
                            value;
                        })) {
                        Anim_RestartSequence(&p->sequence, p->f218, 0x1D, (u8)p->f1D8, 0);
                        p->f1D6 = 2;
                    }
                }
            }
            Collider_SetPosition(p->attackCollider, &p->sprite.position, 0);
            Collider_Refresh(p->attackCollider);
            p->colliderFlags &= ~4;
            p->contactFlags |= 4;
            break;
        case 2: {
            u32 playbackLimit;
            s16 *timer;
            s16 *limit;
            playbackLimit = Screamer_StepRecovery(p);
            timer = &p->f1CA;
            limit = &p->f1CC;
            /* Keep the playback limit live through recovery-field address setup. */
            if (*timer > *limit) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1A, (u8)p->f1D8, 0);
                p->update = Screamer_WaitForPlayerApproach;
                p->f1D6 = 0;
                *timer = 0;
                p->f1D2 = 0;
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
        }
    }
    p->sprite.flags &= ~1;
    p->f1CA++;
    return 0;
}
