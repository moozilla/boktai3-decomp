#include "global.h"
#include "ram.h"
struct AnimPanel {
    u8 unk0[0x34c];
    u32 selectionSecondary;
    u32 selectionPrimary;
    u8 unk354[0x770 - 0x354];
    u32 selections[24];
};
void AnimPanel_ResetSelections(struct AnimPanel *p)
{
    s32 i = 0;
    u8 *saved = gAnimPanelSavedSelections;
    u32 empty = 0x41;
    u8 *out = saved;
    u32 *values = p->selections;
    do {
        *values++ = empty;
        *(u8 *)(i + (u32)out) = empty;
    } while (++i <= 23);
    p->selectionPrimary = 0;
    p->selectionSecondary = 0;
    saved[24] = 0;
    saved[25] = 0;
}
