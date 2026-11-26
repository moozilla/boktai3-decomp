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
u32 ForgingBook_IsWeaponDiscovered(s32 n)
{
    u32 *p;
    if (n > 0x1f) {
        p = &gGameState->a730;
        n -= 0x20;
    } else {
        p = &gGameState->a72c;
    }
    return *p & (1 << n);
}
