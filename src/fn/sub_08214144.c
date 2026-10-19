#include "global.h"

#ifdef NONMATCHING
// fuzzy: 96.4%
// foundry draft (worker best 96.4%); what differs is mostly register allocation and instruction order
extern u32 gUnk_03001110;
extern u32 gUnk_03003A4C;
extern u8 gUnk_03004268;
void m4aSoundMain(void);

void sub_08214144(void)
{
    if (gUnk_03001110 == 0) {
        if (gUnk_03003A4C != 0) {
            *(vu16 *)0x04000004 &= 0xff;
            *(vu16 *)0x04000004 |= 0x9000;
            gUnk_03001110 = 1;
        }
        m4aSoundMain();
    } else {
        u32 m = 0xff;
        *(vu16 *)0x04000010 = gUnk_03004268;
        *(vu16 *)0x04000004 = m & *(vu16 *)0x04000004;
        *(vu16 *)0x04000004 |= 0x4000;
        gUnk_03001110 = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08214144);
#endif
