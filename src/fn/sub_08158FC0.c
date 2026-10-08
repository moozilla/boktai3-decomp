#include "global.h"

extern u8 *gUnk_02000580;
void sub_08158F70(u8 *);
void sub_0813AFEC(u8 *, s32, s32);

s32 sub_08158FC0(void)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        sub_08158F70(g);
        sub_0813AFEC(g, 0xB, 0);
    }
    return 0;
}
