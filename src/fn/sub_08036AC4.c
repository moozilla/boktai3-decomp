#include "global.h"

struct S {
    u8 f[0x19];
    u8 v;
};
extern struct S *gUnk_02000484;

u8 Wireless_GetConnectedSlotMask(void)
{
    if (gUnk_02000484 == 0)
        return 0;
    return gUnk_02000484->v;
}
