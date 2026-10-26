#include "global.h"
#include "engine.h"
#include "ram.h"
void Player_GetWorldPosition(u32, s32 *);
static inline s32 absolute(s32 x)
{
    if (x < 0)
        x = -x;
    return x;
}
u32 Collectible_IsNearPlayer(u8 *s)
{
    s32 v[4];
    s32 i;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        i = 0;
        s += 0xDC;
        do {
            Player_GetWorldPosition(i, v);
            if (absolute((v[0] >> 12) - *(s16 *)s) <= 160)
                return 1;
            ++i;
        } while (i <= 1);
    fail:
        return 0;
    } else {
        Player_GetWorldPosition(0, v);
        if (absolute((v[0] >> 12) - *(s16 *)(s + 0xDC)) > 160)
            goto fail;
    }
    return 1;
}
