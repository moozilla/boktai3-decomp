#include "player_cue.h"
#include "player_cue_sequence.h"
// confidence=low: tentative cue role from callback phases.

void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_CompleteAction(struct Player *);
void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);

#include "scene_vec.h"
static inline void Face(struct Player *p, u32 phase)
{
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = phase;
    }
}

// confidence=low: reduces arrival scale and returns to standing after animation.
void PlayerCue_UpdateSideArrival(struct Player *p)
{
    u32 phase = p->tetherPhase;
    switch ((s16)phase) {
        case 0:
            if (!p->tetherFrame) {
                Face(p, phase);
                ++p->tetherFrame;
            }
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             2, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            if (PlayerCue_Graphics(p)->verticalScale > 30)
                PlayerCue_Graphics(p)->verticalScale -= 30;
            else {
                PlayerCue_Graphics(p)->verticalScale = 0;
                Sound_PlaySong(0x1B0);
                ActorSequence_Begin(p, 0x1C, 1);
            }
            break;
        case 1:
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             3, p->heading, p->side);
            if (PlayerCue_StepSequenceEndedWithFirstFlipMask(
                    &PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence, phase)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
                p->triggerAnimationCallback = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
