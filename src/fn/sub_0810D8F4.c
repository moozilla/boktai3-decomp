#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.5%
// foundry draft (worker best 80.5%); what differs is mostly register allocation and instruction order
#define RecordPanel_RefreshEntryLabels sub_0810D8F4
#define TextWindow_Hide                TextWindow_Clear
#define TextWindow_Show                TextWindow_StartPlayback
struct RecordPanel {
    u8 pad0[0xCEC];
    const u8 *labelScript;
    u8 padCF0[0x18];
    s32 windows[7];
    u8 padD24[2];
    u8 mode;
};
extern const u8 gUnk_0860654C[];
void TextWindow_Clear(u32);
void TextWindow_StartPlayback(s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
u8 StateFlags_TestIndexedFlag(s32);
void sub_0810D8F4(struct RecordPanel *p)
{
    s32 skip;
    s32 base;
    s32 next;
    s32 i;
    switch ((s8)p->mode) {
        case 0:
            skip = gUnk_0860654C[0] + 1;
            base = 0;
            break;
        case 1:
            skip = gUnk_0860654C[1] + 1;
            base = 6;
            break;
        default:
            return;
    }
    for (i = 0; i <= 5; i = next) {
        if (skip == i) {
            next = i + 1;
            TextWindow_Clear(p->windows[next]);
        } else {
            next = i + 1;
            TextWindow_Clear(p->windows[next]);
            TextWindow_SetScript(p->windows[next], p->labelScript);
            i += base;
            if (StateFlags_TestIndexedFlag(i))
                TextWindow_Refresh(p->windows[next], i);
            else
                TextWindow_Refresh(p->windows[next], 12);
            TextWindow_StartPlayback(p->windows[next]);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810D8F4);
#endif
