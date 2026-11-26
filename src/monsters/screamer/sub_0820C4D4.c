#include "global.h"
#include "monsters/screamer/screamer_playback.h"
#include "player/cues/player_cue_sequence.h"
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

s32 Screamer_UpdateScream(struct ScreamerState *p)
{
    s32 phase;
    p->sprite.flags &= ~1;
    phase = p->f1D6;
    switch (phase) {
        case 0:
            Screamer_StepAnimation(p);
            if (p->f1CA > p->f1D4) {
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
                    u16 *count = (u16 *)&p->f1D2;
                    u32 next;
                    s16 *limit;
                    next = *count + 1;
                    *count = next;
                    limit = &p->f1D0;
                    if ((s16)next >= *limit) {
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
            playbackLimit = Screamer_StepAnimation(p);
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
