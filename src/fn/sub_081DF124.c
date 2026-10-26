#include "global.h"
#include "ram.h"
struct AnimPanel {
    u8 unk0[0x34c];
    u32 selectionSecondary;
    u32 selectionPrimary;
    u8 unk354[0x770 - 0x354];
    u32 selections[24];
};
void AnimPanel_LoadSelections(struct AnimPanel *p)
{
    s32 i = 0;
    u8 *saved = gAnimPanelSavedSelections;
    u8 *out = saved;
    u32 *values = p->selections;
    do {
        *values++ = *(u8 *)(i + (u32)out);
    } while (++i <= 23);
    p->selectionPrimary = saved[24];
    p->selectionSecondary = saved[25];
}
