#include "global.h"
struct AnimPanelToggle {
    u8 unk0[0xac];
    u32 firstFlags;
    u8 unkB0[0x134 - 0xb0];
    u32 secondFlags;
    u8 unk138[0x1b4 - 0x138];
    s32 selection;
};
void AnimPanel_SelectActiveAnimation(struct AnimPanelToggle *p)
{
    if (p->selection == 0) {
        p->firstFlags &= ~1;
        p->secondFlags |= 1;
    } else {
        p->firstFlags |= 1;
        p->secondFlags &= ~1;
    }
}
