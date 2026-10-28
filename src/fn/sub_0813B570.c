#include "global.h"

struct P0813B570 {
    u8 f0[0x332];
    s16 a;
    u8 f334[0x41c - 0x334];
    u16 b;
};
struct G0813B570 {
    u8 f0[0x2a];
    s16 v;
};
extern u32 gEngineFlags;
extern struct G0813B570 *gGameState;

s32 Player_GetMaxLife(struct P0813B570 *p)
{
    s32 s;
    if (gEngineFlags & 0x1000)
        return gGameState->v;
    {
        u8 *q = (u8 *)p + 0x32C;
        s = *(s16 *)(q + 6) + *(u16 *)(q + 0xF0);
    }
    if (s > 0x78)
        s = 0x78;
    return s * 10;
}
