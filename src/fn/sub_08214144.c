#include "global.h"
#include "runtime.h"

extern u32 gUnk_03003A4C;
extern u8 gUnk_03004268;
void m4aSoundMain(void);

void sub_08214144(void)
{
    if (gVideoIrqPhase == 0) {
        if (gUnk_03003A4C != 0) {
            *(vu16 *)0x04000004 &= 0xff;
            *(vu16 *)0x04000004 |= 0x9000;
            gVideoIrqPhase = 1;
        }
        m4aSoundMain();
    } else {
        vu16 *r = (vu16 *)0x04000010;
        u8 *q = &gUnk_03004268;
        u32 m = 0xff;
        *r = *q;
        *(vu16 *)0x04000004 = m & *(vu16 *)0x04000004;
        *(vu16 *)0x04000004 |= 0x4000;
        gVideoIrqPhase = 0;
    }
}
