#include "global.h"
#include "scene_pool.h"
#define ScanlineScroll_AdvancePhases sub_081111E8
struct ScanlineScroll {
    u8 unknown0[0x1858];
    u16 offsets[160];
    u16 phases[160];
    u16 speedTop, speedBottom;
    u16 unknown1adc;
    u16 split;
};
static inline u8 IsPlayerScrollBlocked(void)
{
    if (gUnk_020004F0[0]) {
        u32 flags = gUnk_020004F0[0]->flags;
        if (flags & 5) return 1;
    }
    return 0;
}
void sub_081111E8(struct ScanlineScroll *p)
{
    s32 i;
    if (!IsPlayerScrollBlocked()) {
        i = 0;
        for (; i < p->split; i++) {
            u32 speed = p->speedTop;
            p->phases[i] = (p->phases[i] + speed) & 0xfff;
            p->offsets[i] = p->phases[i] >> 7;
        }
        for (; i < 160; i++) {
            u32 speed = p->speedBottom;
            p->phases[i] = (p->phases[i] + speed) & 0xfff;
            p->offsets[i] = p->phases[i] >> 7;
        }
    }
}
