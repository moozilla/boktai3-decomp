#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.4%
// Open: only the reload register for the 0x18 offset differs (ROM r5, here r1);
// rasolve says one fewer reload must precede it.
struct StatBlock {
    u8 pad[0x18];
    s16 base[4];
    s16 bonus[4];
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern struct StatBlock *gUnk_02000710;
s32 Mod(s32, s32);
static inline int nextRandom(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
s32 SceneHud_SelectUpgradeableStat(s32 amount)
{
    s32 start, i, a;
    struct StatBlock *g;
    a = amount;
    start = Mod(nextRandom() >> 3, 3);
    g = gUnk_02000710;
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
#else
INCLUDE_ASM("asm/nonmatching", SceneHud_SelectUpgradeableStat);
#endif
