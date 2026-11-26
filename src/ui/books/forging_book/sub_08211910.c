#include "global.h"
struct G {
    u8 pad[0x720];
    u32 a720;
    u32 a724;
    u32 a728;
    u32 a72c;
    u32 a730;
};
extern struct G *gGameState;
u32 ForgingBook_IsItemDiscovered(s32 n)
{
    u32 a = gGameState->a720 | 0x04000400;
    u32 b = gGameState->a724 | 0x8020;
    if (n <= 0x1f)
        return a & (1 << n);
    return b & (1 << (n - 0x20));
}
