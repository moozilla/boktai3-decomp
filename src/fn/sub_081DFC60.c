#include "global.h"
#include "ram.h"

struct AnimPanel {
    u8 unk0[0x34c];
    s32 selectionSecondary;
    s32 selectionPrimary;
    u8 unk354[0x770 - 0x354];
    s32 selections[24];
};
void sub_081DFB18(struct AnimPanel *, s32);
void Password_BuildLabels(struct AnimPanel *);
s32 AnimPanel_EraseSelection(struct AnimPanel *p)
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
            *(u32 *)((u32)entries + (p->selectionPrimary << 2)) = value;
        }
    } else {
        *entry = 0x41;
    }
    sub_081DFB18(p, p->selectionPrimary);
    Password_BuildLabels(p);
    return empty;
}
