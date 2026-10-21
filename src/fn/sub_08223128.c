#include "global.h"

#define gSerialConnectionRole gUnk_0300531C
#define gSerialTransferStatus gUnk_03005324
extern s32 gUnk_0300531C, gUnk_03005324;
void Serial_CheckMultiplayerReady(void)
{
    u32 status;
    gUnk_0300531C = -1;
    gUnk_03005324 = 0;
    status = *(vu16 *)0x04000128;
    if ((u16)(status & 0x88) != 8) {
        gUnk_03005324 = -1;
    } else {
        if ((u16)(status & 4) == 0)
            gUnk_0300531C = 0;
        else
            gUnk_0300531C = -1;
        *(vu16 *)0x0400012A = 0x4000;
    }
}
