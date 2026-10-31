#include "player_cue.h"
#include "player_cue_sequence.h"
// confidence=low: tentative player cue role; see callback body.

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdateHidden(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);

extern const s16 gUnk_086149C4[];
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_SetAnimation(struct Player *, s32, s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

#include "scene_vec.h"
#include "render.h"
void Camera_SnapToWithoutCompletion(struct SceneVec *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline void Face(struct Player *p)
{
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
}
// confidence=low: camera lift and scale descent precede hidden cue state.
void PlayerCue_UpdateSideDeparture(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0: {
            struct SceneVec pos;
            Player_AddControlFlags(p, 4);
            pos = p->position;
            pos.y += 150;
            Camera_SnapToWithoutCompletion(&pos);
            if (p->cueVariant <= 1) {
                if (p->cueVariant)
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 16);
                else
                    ScreenTransition_SendCommand(3, 3, 4, 4, 4, 0x1FFF, 1);
            }
            Face(p);
            Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource,
                             2, p->heading, p->side);
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite,
                                   &PlayerCue_Graphics(p)->sequence);
            Sound_PlaySong(0x1AD);
            ActorSequence_Begin(p, 0x1B, 1);
            PlayerCue_Graphics(p)->verticalScale = 0;
            break;
        }
        case 1:
            Player_AddControlFlags(p, 4);
            if (++p->tetherFrame > 5) {
                PlayerCue_Graphics(p)->verticalScale = 0;
                PlayerCue_Graphics(p)->sprite.priority = 1;
                Sound_PlaySong(0x1AE);
                ActorSequence_Begin(p, 0x1B, 2);
            }
            break;
        case 2:
            Player_AddControlFlags(p, 4);
            PlayerCue_Graphics(p)->verticalScale -= 60;
            if (++p->tetherFrame > 29) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0x1B, 3);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 3:
            Player_AddControlFlags(p, 5);
            break;
    }
}
