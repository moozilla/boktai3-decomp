#include "global.h"
#include "engine.h"

s32 Player_GetMaxEnergy(u8 *p)
{
    u8 *q = p + 0x32C;
    s32 s;
    if (gEngineFlags & 0x1000)
        return 0xc;
    s = *(s16 *)(q + 8) + *(u16 *)(p + 0x41E);
    if (s > 0x78)
        s = 0x78;
    return s * 5 + 100;
}
