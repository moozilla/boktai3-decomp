#include "global.h"
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
void RecordPanel_RefreshEntryLabels(struct RecordPanel *p)
{
    s32 skip, base, i;
    s32 *windows;
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
    i = 0;
    windows = p->windows;
    for (; i <= 5;) {
        s32 next;
        if (skip == i) {
            next = i + 1;
            TextWindow_Clear(windows[next]);
            i = next;
        } else {
            s32 *w;
            next = i + 1;
            w = &windows[next];
            TextWindow_Clear(*w);
            TextWindow_SetScript(*w, p->labelScript);
            i += base;
            if (StateFlags_TestIndexedFlag(i))
                TextWindow_Refresh(*w, i);
            else
                TextWindow_Refresh(*w, 12);
            TextWindow_StartPlayback(windows[next]);
            i = next;
        }
    }
}
