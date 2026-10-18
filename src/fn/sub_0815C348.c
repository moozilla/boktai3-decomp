#include "global.h"
void Player_EnterKindTwo(u8 *);
void ActorSequence_Begin(u8 *, s32, s32);
void PlayerCue_UpdateFaceSide(void);
/* Sets the actor's +0x3A4 mode to 5 (on) or 3 and +0x3A2/+0x3A3, then
   update PlayerCue_UpdateFaceSide. */
void PlayerCue_FaceSide(u8 *a, u32 on)
{
    if (on) {
        u32 zero;
        u8 *mode = &a[0x3A4];
        zero = 0;
        *mode = 5;
        a[0x3A2] = zero;
        a[0x3A3] = 1;
    } else {
        a[0x3A4] = 3;
        a[0x3A2] = on;
        a[0x3A3] = on;
    }
    Player_EnterKindTwo(a);
    ActorSequence_Begin(a, 0, 0);
    *(void **)(a + 0x58C) = PlayerCue_UpdateFaceSide;
}
