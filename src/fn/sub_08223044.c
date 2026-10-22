#include "global.h"
extern u32 gUnk_03005324;
extern s32 gUnk_0300531C;
void InterruptsAndVideo_WaitForVBlank(void);
void sub_08223044(void)
{
    gUnk_03005324 = 0;
    gUnk_0300531C = -1;
    *(vu16 *)0x04000208 = 0;
    *(vu16 *)0x04000200 &= 0xFF7F;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000202 = 0x80;
    InterruptsAndVideo_WaitForVBlank();
    *(vu16 *)0x04000134 = 0;
    *(vu16 *)0x04000128 = 0x2000;
    *(vu16 *)0x04000128 |= 3;
}
