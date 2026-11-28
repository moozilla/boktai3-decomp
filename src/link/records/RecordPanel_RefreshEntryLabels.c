#include "global.h"
#include "ui/widgets/selection_panel.h"
extern const u8 gUnk_0860654C[];
void TextWindow_StartPlayback(s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
u8 StateFlags_TestIndexedFlag(s32);
void RecordPanel_RefreshEntryLabels(struct SelectionPanel *panel)
{
    s32 skip, base, i;
    switch (panel->page) {
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
            TextWindow_Clear(panel->windows[i + 1]);
        } else {
            TextWindow_Clear(panel->windows[i + 1]);
            TextWindow_SetScript(panel->windows[i + 1], panel->contextScript);
            if (StateFlags_TestIndexedFlag(i + base))
                TextWindow_Refresh(panel->windows[i + 1], i + base);
            else
                TextWindow_Refresh(panel->windows[i + 1], 12);
            TextWindow_StartPlayback(panel->windows[i + 1]);
        }
    }
}
