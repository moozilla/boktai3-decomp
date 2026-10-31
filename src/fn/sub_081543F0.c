#include "player_cue.h"
#include "player_cue_sequence.h"

// confidence=low: tentative cue role from callback phases.

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdateHidden(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);

void PlayerCue_UpdateSideFade(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (!p->tetherFrame) {
                p->facing = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             0, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            if ((++p->tetherFrame) > 27)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1:
            PlayerCue_Graphics(p)->sprite.flags |= 2;
            PlayerCue_Graphics(p)->sprite.unknown8[0] = 64 - (((u8)p->tetherFrame) << 2);
            if (p->tetherFrame > 8) {
                PlayerCue_Graphics(p)->sprite.unknown8[1] = p->tetherFrame << 3;
                PlayerCue_Graphics(p)->verticalScale = (p->tetherFrame - 8) * 20;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->tetherFrame) > 15)
                ActorSequence_Begin(p, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(p, 4);
            PlayerCue_Graphics(p)->verticalScale = (p->tetherFrame * 200) + 160;
            if ((++p->tetherFrame) > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                p->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;

        case 3:
            if (!p->tetherFrame) {
                p->facing = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             0, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            Player_AddControlFlags(p, 4);
            PlayerCue_Graphics(p)->sprite.flags |= 2;
            PlayerCue_Graphics(p)->sprite.unknown8[0] = 4;
            PlayerCue_Graphics(p)->sprite.unknown8[1] = 120;
            PlayerCue_Graphics(p)->verticalScale = ((8 - p->tetherFrame) * 200) + 160;
            if ((++p->tetherFrame) > 7)
                ActorSequence_Begin(p, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             0, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            PlayerCue_Graphics(p)->sprite.flags |= 2;
            PlayerCue_Graphics(p)->sprite.unknown8[0] = 64 - ((15 - ((u8)p->tetherFrame)) << 2);
            if (p->tetherFrame <= 7) {
                PlayerCue_Graphics(p)->sprite.unknown8[1] = (15 - p->tetherFrame) << 3;
                PlayerCue_Graphics(p)->verticalScale = (8 - p->tetherFrame) * 20;
            } else {
                PlayerCue_Graphics(p)->sprite.unknown8[1] = 64;
                PlayerCue_Graphics(p)->verticalScale = 0;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->tetherFrame) > 15)
                ActorSequence_Begin(p, 7, 5);
            break;

        case 5:
            PlayerCue_Graphics(p)->sprite.flags &= ~2;
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             0, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            if ((++p->tetherFrame) > 3) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->triggerAnimationCallback = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
