#include "global.h"
#include "ram.h"
extern void (*const gUnk_086107A0[])(void);
extern u32 gUnk_02000560;
void sub_08249240(u8 *, u8 *, void (*)(void));
static inline u8 test(u32 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
u32 FeatherEffectPool_UpdateTask(u8 *s)
{
    u8 *first = s + 0x3C;
    if (*(u32 *)(s + 0x38) && !Paused() && !Locked()) {
        {
            {
                s32 i = 0;
                void (*const *table)(void) = gUnk_086107A0;
                u8 *kind = s + 0xCE;
                u8 *entry = first;
                do {
                    // the do/while(0) raises the call's loop depth, which is
                    // what gives the ROM's allocation (found by tools/autosolve.py)
                    do {
                        if (test((u32 *)(s + 0x38), 1 << i))
                            sub_08249240(s, entry, table[*kind]);
                    } while (0);
                    kind += 0xA0;
                    entry += 0xA0;
                    ++i;
                } while (i <= 19);
            }
        }
    }
    return 0;
}
