#include "player.h"
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateFaceSide(struct Player *);
/* Sets the actor's +0x3A4 mode to 5 (on) or 3 and +0x3A2/+0x3A3, then
   update PlayerCue_UpdateFaceSide. */
void PlayerCue_FaceSide(struct Player *p, u32 on)
{
    if (on) {
        u32 zero;
        u8 *mode = &p->facing;
        zero = 0;
        *mode = 5;
        p->heading = zero;
        p->side = 1;
    } else {
        p->facing = 3;
        p->heading = on;
        p->side = on;
    }
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateFaceSide;
}
