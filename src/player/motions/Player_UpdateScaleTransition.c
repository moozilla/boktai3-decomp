#include "global.h"
#include "player/player.h"
#include "player/player_status_flags.h"

u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetPendingCallback(struct Player *, void (*)(struct Player *));
void Player_OnFadeMotionExit(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void Player_SetActionPhase(struct Player *, u8);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateScaleTransition(struct Player *p)
{
    struct AnimSprite *animation;
    if (Player_ConsumeStateEntry(p))
        Player_SetPendingCallback(p, Player_OnFadeMotionExit);
    animation = &p->animation;
    animation->flags |= 2;
    Player_AddStatusFlags(p, 0x40000);
    p->statusFlags |= 0x1100;
    Player_AddControlFlags(p, 28);
    switch (p->tetherPhase) {
        case 0:
            if (p->tetherFrame <= 63) {
                p->animation.scaleY = p->animation.scaleX = 64 - p->tetherFrame;
                p->tetherFrame++;
            } else
                Player_SetActionPhase(p, 1);
            break;
        case 1:
            Player_AddControlFlags(p, 1);
            break;
        case 2:
            animation->flags &= ~1;
            if (++p->tetherFrame < 64) {
                p->animation.scaleY = p->animation.scaleX = p->tetherFrame;
            } else {
                animation->flags &= ~2;
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
