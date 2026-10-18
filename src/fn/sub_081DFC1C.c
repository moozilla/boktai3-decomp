#define AnimPanel_AppendSelection sub_081DFC1C
#define gAnimPanelSavedSelections gUnk_020005C0
#include "global.h"
extern u8 gUnk_020005C0[];
struct AnimPanel {
    u8 unk0[0x34c];
    s32 selectionSecondary;
    s32 selectionPrimary;
    u8 unk354[0x770-0x354];
    s32 selections[24];
};
void sub_081DFB18(struct AnimPanel *, s32);
void sub_081DFA50(struct AnimPanel *);
s32 sub_081DFC1C(struct AnimPanel *p)
{
    s32 full;
    s32 index;
    s32 *cursor = &p->selectionPrimary;
    p->selections[p->selectionPrimary] = p->selectionSecondary;
    index = *cursor;
    if (index <= 22) {
        *cursor = 1 + index;
        full = 0;
    } else {
        full = 1;
    }
    sub_081DFB18(p, index);
    sub_081DFA50(p);
    return full;
}
