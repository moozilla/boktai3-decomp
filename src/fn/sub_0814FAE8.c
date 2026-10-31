#include "global.h"
#include "player.h"
void Player_UpdateFacingSide(struct Player *);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_EnterStandUpdate(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_UpdateHidden(struct Player *);
void Player_UpdateTransitionAnimation(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, 0x15F, 0x40, 1)) {
                p->tetherPhase = 1;
                goto dispatch;
            }
            break;
        case 1:
            Player_SetAnimation(p, 0x160, 0x40, 1);
            break;
        case 2:
            if (Player_SetAnimation(p, 0x15F, 0x40, 1)) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                goto dispatch;
            }
            break;
        case 3:
            Player_SetAnimation(p, 0x160, 0x40, 1);
            {
                u32 frame = p->tetherFrame;
                s32 mask = 7;
                s32 low;
                /* Low three frame bits. */
                low = ~(~frame | ~mask);
                frame >>= 3;
                {
                    s32 threshold = mask - frame;
                    if (low < threshold)
                        Player_AddControlFlags(p, 5);
                }
            }
            if (++p->tetherFrame > 55) {
                ActorSequence_Begin(p, 0, 1);
            dispatch:
                PlayerCue_CompleteAction(p);
            }
            break;
        case 4:
            Player_SetAnimation(p, 0x160, 0x40, 1);
            {
                u32 frame = p->tetherFrame;
                s32 mask = 7;
                s32 low;
                /* Low three frame bits. */
                low = ~(~frame | ~mask);
                frame >>= 3;
                {
                    s32 threshold = mask - frame;
                    if (low >= threshold)
                        Player_AddControlFlags(p, 5);
                }
            }
            if (++p->tetherFrame > 55) {
                ActorSequence_Begin(p, 0, 0);
                p->triggerAnimationCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
