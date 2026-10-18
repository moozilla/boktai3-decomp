#include "global.h"

void sub_0813AFEC(u8 *, s32, s32);
void sub_0814F90C(void);

void PlayerCue_EnterStandUpdate(u8 *p)
{
    sub_0813AFEC(p, 0, 0);
    *(void (**)(void))(p + 0x58C) = sub_0814F90C;
}
