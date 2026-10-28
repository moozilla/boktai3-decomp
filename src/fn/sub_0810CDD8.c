#include "global.h"
#include "area_effect_link.h"
struct PanelSelection {
    u8 pad0[0x53];
    volatile u8 type;
    u8 pad54[0xEC];
    u8 text120[0x20];
    u8 text140[0x20];
    u8 text160[0x20];
    u8 text180[0x20];
    u8 text1A0[0x20];
    u8 text1C0[0x20];
};
struct LinkPanel {
    u8 pad0[0x13F4];
    void *script;
    u8 pad13F8[8];
    s32 window0;
    s32 window1;
};
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
void LinkPanel_StartDescriptions(struct LinkPanel *p)
{
    struct PanelSelection *sel = gLinkSession;
    if (sel != 0 && sel->type == 1 && (void *)((u8 *)sel + 0x120) != 0) {
        s32 *w = &p->window0;
        TextWindow_SetScript(*w, p->script);
        TextWindow_Refresh(*w, 0);
        TextWindow_SetString(*w, 1, (u8 *)sel + 0x130);
        TextWindow_StartPlayback(*w);
        sel = gLinkSession;
        if (sel != 0 && sel->type == 1 && (void *)((u8 *)sel + 0x160) != 0) {
            w = &p->window1;
            TextWindow_SetScript(*w, p->script);
            TextWindow_Refresh(*w, 1);
            TextWindow_SetString(*w, 2, (u8 *)sel + 0x170);
            TextWindow_StartPlayback(*w);
        }
    }
}
