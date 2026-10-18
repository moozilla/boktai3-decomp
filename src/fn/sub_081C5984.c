#include "global.h"

extern u16 gUnk_03004BD8;
void sub_08216520(u32, u32, u32);

void ChallengeMenu_EnableBackground(void)
{
    u32 m = 0x100;
    gUnk_03004BD8 = m | gUnk_03004BD8;
    sub_08216520(1, 0, 0);
}
