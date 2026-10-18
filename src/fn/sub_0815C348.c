#include "global.h"
void sub_0815AD78(u8 *);
void sub_0813AFEC(u8 *, s32, s32);
void sub_0814FC54(void);
/* Sets the actor's +0x3A4 mode to 5 (on) or 3 and +0x3A2/+0x3A3, then
   update sub_0814FC54. */
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
    sub_0815AD78(a);
    sub_0813AFEC(a, 0, 0);
    *(void **)(a + 0x58C) = sub_0814FC54;
}
