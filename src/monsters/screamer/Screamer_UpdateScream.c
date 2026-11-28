#include "global.h"
#include "monsters/screamer/screamer_playback.h"
#include "player/cutscene/player_cue_sequence.h"
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);

s32 Screamer_UpdateScream(struct ScreamerState *screamer)
{
    s32 phase;
    screamer->sprite.flags &= ~1;
    phase = screamer->f1D6;
    switch (phase) {
        case 0:
            Screamer_StepAnimation(screamer);
            if (screamer->f1CA > screamer->f1D4) {
                Anim_RestartSequence(&screamer->sequence, screamer->f218, 0x1B, (u8)screamer->f1D8,
                                     0);
                Anim_SetDurationScale(&screamer->sequence, 0x20);
                screamer->f1D6 = 1;
                Sound_PlaySong(0x51A);
            }
            screamer->bodyCollider.flags &= ~4;
            Collider_SetPosition(&screamer->bodyCollider, &screamer->sprite.position, 0);
            screamer->frontCollider.flags |= 4;
            break;
        case 1:
            if (PlayerCue_StepSequenceEndedWithFirstFlipMask(&screamer->sprite, &screamer->sequence,
                                                             phase)) {
                if (++screamer->f1D2 >= screamer->f1D0) {
                    Anim_RestartSequence(&screamer->sequence, screamer->f218, 0x1D,
                                         (u8)screamer->f1D8, 0);
                    screamer->f1D6 = 2;
                }
            }
            Collider_SetPosition(&screamer->attackCollider, &screamer->sprite.position, 0);
            Collider_Refresh(&screamer->attackCollider);
            screamer->bodyCollider.flags &= ~4;
            screamer->frontCollider.flags |= 4;
            break;
        case 2:
            Screamer_StepAnimation(screamer);
            if (screamer->f1CA > screamer->f1CC) {
                Anim_RestartSequence(&screamer->sequence, screamer->f218, 0x1A, (u8)screamer->f1D8,
                                     0);
                screamer->update = Screamer_WaitForPlayerApproach;
                screamer->f1D6 = 0;
                screamer->f1CA = 0;
                screamer->f1D2 = 0;
            }
            screamer->bodyCollider.flags &= ~4;
            Collider_SetPosition(&screamer->bodyCollider, &screamer->sprite.position, 0);
            screamer->frontCollider.flags |= 4;
            break;
    }
    screamer->sprite.flags &= ~1;
    screamer->f1CA++;
    return 0;
}
