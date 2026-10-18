#include "global.h"

extern u8 *gUnk_02000484;

void Wireless_RequestResetConnection(void)
{
    if (gUnk_02000484) {
        gUnk_02000484[0x3D] = 1;
    }
}
