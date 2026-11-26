#include "engine/shared/global.h"
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
    for (i = 0; i < 6; i++) {
        if (skip == i) {
            TextWindow_Clear(p->windows[i + 1]);
        } else {
            TextWindow_Clear(p->windows[i + 1]);
            TextWindow_SetScript(p->windows[i + 1], p->labelScript);
            if (StateFlags_TestIndexedFlag(i + base))
                TextWindow_Refresh(p->windows[i + 1], i + base);
            else
                TextWindow_Refresh(p->windows[i + 1], 12);
            TextWindow_StartPlayback(p->windows[i + 1]);
        }
    }
}
