#define gSerialStatus gUnk_03005324
#define gSerialRole   gUnk_0300531C
#include "global.h"
extern s32 gUnk_03005324;
extern s32 gUnk_0300531C;
void Serial_ProbeConnection(void)
{
    vu16 *send = (vu16 *)0x0400012A;
    u16 busy;
    u16 control;
    *send = 0;
    control = *(vu16 *)0x04000128;
    if (!(control & 8)) {
        gUnk_03005324 = -9;
    } else {
        busy = control & 0x80;
        if (busy) {
            gUnk_03005324 = -4;
        } else {
            if (!(control & 4))
                gUnk_0300531C = 0;
            else
                gUnk_0300531C = -1;
            *send = busy;
        }
    }
}
