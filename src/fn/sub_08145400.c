#include "global.h"
#include "player.h"

#define Player_ConsumePhaseEntry     Player_ConsumeStateEntry
#define Player_SetSequenceCallback   Player_SetPendingCallback
#define Player_ClearAnimationFlagTwo Player_OnFadeMotionExit
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetPendingCallback(struct Player *, void (*)(struct Player *));
void Player_OnFadeMotionExit(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void Player_SetActionPhase(struct Player *, u8);
void ActorSequence_Begin(struct Player *, s32, s32);
static inline void SetStatusFlags(struct Player *p, u32 mask)
{
    p->statusFlags |= mask;
}
void Player_UpdateScaleTransition(struct Player *p)
{
    struct PlayerAnimation *animation;
    if (Player_ConsumeStateEntry(p))
        Player_SetPendingCallback(p, Player_OnFadeMotionExit);
    animation = &p->animation;
    animation->flags |= 2;
    SetStatusFlags(p, 0x40000);
    p->statusFlags |= 0x1100;
    Player_AddControlFlags(p, 28);
    switch (p->tetherPhase) {
        case 0: {
            u16 frame = p->tetherFrame;
            if (frame <= 63) {
                s32 scale = 64 - frame;
                p->movementSpeedScale = scale;
                p->secondarySpeedScale = scale;
                p->tetherFrame++;
            } else
                Player_SetActionPhase(p, 1);
        } break;
        case 1:
            Player_AddControlFlags(p, 1);
            break;
        case 2:
            animation->flags &= ~1;
            {
                s32 n = p->tetherFrame + 1;
                long long scale = n;
                if ((p->tetherFrame = n) <= 63) {
                    p->movementSpeedScale = scale;
                    p->secondarySpeedScale = scale;
                } else {
                    animation->flags &= ~2;
                    ActorSequence_Begin(p, 0, 0);
                }
            }
            break;
    }
}
