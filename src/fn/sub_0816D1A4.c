#include "global.h"
#include "game_state.h"
#include "engine.h"
struct StatBlock {
    u8 pad[0x18];
    s16 base[4];
    s16 bonus[4];
};
s32 Mod(s32, s32);
static inline int nextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
s32 SceneHud_SelectUpgradeableStat(s32 amount)
{
    s32 start, i, a;
    struct StatBlock *g;
    a = amount;
    start = Mod(nextRandom() >> 3, 3);
    g = gGameState;
    {
        s16 *e = (s16 *)(start * 2 + (u32)g);
        if (e[12] + e[16] + a > 100)
            a = 100 - e[12] - e[16];
    }
    if (a > 0)
        return start;
    for (i = 0; i <= 2; i++) {
        a = amount;
        {
            s16 *e = (s16 *)(i * 2 + (u8 *)g);
            if (e[12] + e[16] + a > 100)
                a = 100 - e[12] - e[16];
        }
        if (a > 0)
            return i;
    }
    return 0xff;
}
