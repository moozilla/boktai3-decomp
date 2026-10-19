
#ifdef NONMATCHING
// fuzzy: 97.5%
// foundry draft (worker best 97.5%); what differs is mostly register allocation and instruction order
#define AnimPanel_EraseSelection sub_081DFC60
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
s32 sub_081DFC60(struct AnimPanel *p)
{
    s32 empty = 0;
    s32 index = p->selectionPrimary;
    u32 offset = index * 4;
    u32 *entries = p->selections;
    u32 *entry = (u32 *)((u32)entries + offset);
    u32 value = *entry;
    if (value == 0x41) {
        if (index == 0) {
            empty = 1;
        } else {
            p->selectionPrimary = index - 1;
            entries[p->selectionPrimary] = value;
        }
    } else {
        *entry = 0x41;
    }
    sub_081DFB18(p, p->selectionPrimary);
    sub_081DFA50(p);
    return empty;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081DFC60);
#endif
