
#ifdef NONMATCHING
// fuzzy: 93.2%
// foundry draft (worker best 93.2%); what differs is mostly register allocation and instruction order
#define Serial_CheckMultiplayerReady sub_08223128
#define gSerialConnectionRole        gUnk_0300531C
#define gSerialTransferStatus        gUnk_03005324
#include "global.h"
extern s32 gUnk_0300531C, gUnk_03005324;
void sub_08223128(void)
{
    u16 status;
    u16 mask;
    gUnk_0300531C = -1;
    gUnk_03005324 = 0;
    mask = 0x88;
    status = *(vu16 *)0x04000128;
    if ((u16)(status & mask) != 8) {
        gUnk_03005324 = -1;
    } else {
        if ((u16)(status & 4) == 0)
            gUnk_0300531C = 0;
        else
            gUnk_0300531C = -1;
        *(vu16 *)0x0400012A = 0x4000;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08223128);
#endif
