#include "global.h"

extern u8 *gUnk_020004B4;

void Registry1745_RequestHandshakeFinish(void)
{
    if (gUnk_020004B4) {
        gUnk_020004B4[0x2B] = 1;
    }
}
