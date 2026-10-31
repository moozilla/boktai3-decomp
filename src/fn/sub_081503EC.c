// confidence=low: tentative cue role inferred from named callers and callback behavior.
#include "player_cue.h"
// confidence=low: cue callbacks select departure and arrival phases with scale changes.
void Player_AddControlFlags(void *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Sound_PlaySong(u32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_CompleteAction(void *);
void PlayerCue_UpdateHidden(struct Player *);
void PlayerCue_EnterStandUpdate(void *);
void Player_UpdateFacingSide(void *);
#define ANIM(n) Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, n), 64, 1)
static inline void SetScale(struct Player *p, u32 value)
{
    PlayerCue_Graphics(p)->verticalScale = value;
}
void PlayerCue_UpdateScaleTransition(struct Player *p)
{
    u16 *t;
    switch (p->tetherPhase) {
        case 0:
            p->resetValue6FC = 15;
            if (ANIM(8))
                ActorSequence_Begin(p, 7, 1);
            break;
        case 1:
            p->resetValue6FC = 30;
            ANIM(10);
            if (++p->tetherFrame > 15)
                ActorSequence_Begin(p, 7, 2);
            break;
        case 2:
            if (p->actionMode == 6 && !p->tetherFrame)
                Sound_PlaySong(0x327);
            p->resetValue6FC = 30;
            p->animation.flags |= 2;
            t = &p->tetherFrame;
            p->movementSpeedScale = 64 - ((u8)*t << 2);
            if (*t > 8) {
                p->secondarySpeedScale = *t << 3;
                {
                    s32 frame = *t - 8;
                    SetScale(p, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(p, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(p, 7, 3);
            break;

        case 3:
            Player_AddControlFlags(p, 4);
            SetScale(p, p->tetherFrame * 200 + 160);
            if (++p->tetherFrame > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                p->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;
        case 4:
            if (p->actionMode == 6 && !p->tetherFrame)
                Sound_PlaySong(0x201);
            ANIM(0);
            Player_AddControlFlags(p, 4);
            p->animation.flags |= 2;
            p->movementSpeedScale = 4;
            p->secondarySpeedScale = 120;
            SetScale(p, (8 - p->tetherFrame) * 200 + 160);
            if (++p->tetherFrame > 7)
                ActorSequence_Begin(p, 7, 5);
            break;
        case 5:
            ANIM(0);
            p->animation.flags |= 2;
            p->movementSpeedScale = 64 - ((15 - (u8)p->tetherFrame) << 2);
            if (p->tetherFrame <= 7) {
                p->secondarySpeedScale = (15 - p->tetherFrame) << 3;
                SetScale(p, (8 - p->tetherFrame) * 20);
            } else {
                p->secondarySpeedScale = 64;
                SetScale(p, 0);
                Player_AddControlFlags(p, 4);
            }
            if (++p->tetherFrame > 15)
                ActorSequence_Begin(p, 7, 6);
            break;
        case 6:
            p->animation.flags &= ~2;
            ANIM(0);
            if (++p->tetherFrame > 3) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 7:
            if (p->actionMode == 6 && !p->tetherFrame)
                Sound_PlaySong(0x327);
            ANIM(0);
            p->animation.flags |= 2;
            t = &p->tetherFrame;
            p->movementSpeedScale = 64 - ((u8)*t << 2);
            if (*t > 8) {
                p->secondarySpeedScale = *t << 3;
                {
                    s32 frame = *t - 8;
                    SetScale(p, ((frame << 2) + frame) << 2);
                }
                Player_AddControlFlags(p, 4);
            }
            if (++*t > 15)
                ActorSequence_Begin(p, 7, 3);
            break;
        case 8:
            Player_AddControlFlags(p, 4);
            ANIM(0);
            SetScale(p, p->tetherFrame * 200 + 160);
            if (++p->tetherFrame > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                p->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
