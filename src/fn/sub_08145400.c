
#ifdef NONMATCHING
// fuzzy: 95.2%
// foundry draft (worker best 94.2%); what differs is mostly register allocation and instruction order
#define Player_UpdateScaleTransition sub_08145400
#define Player_ConsumePhaseEntry Player_ConsumeStateEntry
#define Player_SetSequenceCallback Player_SetPendingCallback
#define Player_ClearAnimationFlagTwo Player_OnFadeMotionExit
#include "global.h"
#include "player.h"
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
void sub_08145400(struct Player *p)
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
    case 0:
        {
        u16 frame = p->tetherFrame;
        if (frame <= 63) {
            s32 scale = 64 - frame;
            p->movementSpeedScale = scale;
            p->secondarySpeedScale = scale;
            p->tetherFrame++;
        } else
            Player_SetActionPhase(p, 1);
        }
        break;
    case 1:
        Player_AddControlFlags(p, 1);
        break;
    case 2:
        animation->flags &= ~1;
        {
            s32 scale = p->tetherFrame;
            p->tetherFrame++;
            scale++;
            if (p->tetherFrame <= 63) {
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
#else
INCLUDE_ASM("asm/nonmatching", sub_08145400);
#endif
