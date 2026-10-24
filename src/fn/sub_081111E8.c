#include "global.h"
#include "scene_pool.h"
#include "background_controller.h"
struct ScanlineScroll {
    u8 unknown0[0x1848];
    u16 scroll;
    u16 unknown184a;
    u8 layer, moving;
    s16 lastMapX;
    s16 remaining;
    u16 unknown1852;
    s32 speed;
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
        if (flags & 5)
            return 1;
    }
    return 0;
}

// confidence=low: advances each scanline's wave phase, by speedTop above the split line and
// speedBottom below it, and derives the line's scroll offset.
void ScanlineScroll_AdvancePhases(struct ScanlineScroll *p)
{
    s32 i;
    s32 speed;
    u16 topPhase;
    u16 bottomPhase;
    if (IsPlayerScrollBlocked())
        return;
    for (i = 0; i < p->split; i++) {
        speed = p->speedTop;
        topPhase = (p->phases[i] + speed) & 0xFFF;
        p->phases[i] = topPhase;
        p->offsets[i] = p->phases[i] >> 7;
    }

    for (; i < 160; i++) {
        speed = p->speedBottom;
        bottomPhase = (p->phases[i] + speed) & 0xFFF;
        p->phases[i] = bottomPhase;
        p->offsets[i] = p->phases[i] >> 7;
    }
}
