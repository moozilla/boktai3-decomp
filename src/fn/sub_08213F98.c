#include "global.h"
void InterruptsAndVideo_WaitForVBlank(void);
void InterruptsAndVideo_WaitFrames(u16 n)
{
    u16 save = *(vu16 *)0x04000208;
    u16 i;
    *(vu16 *)0x04000208 = 1;
    i = n - 1;
    while (i != 0xFFFF) {
        InterruptsAndVideo_WaitForVBlank();
        i--;
    }
    *(vu16 *)0x04000208 = save;
}
