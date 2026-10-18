#include "global.h"

s32 LinkPlayer_GetElapsedActionTicks(u8 *p)
{
    s32 r = p[0x4F4];
    r -= p[0x4F5];
    r += 0x100;
    return r & 0xFF;
}
