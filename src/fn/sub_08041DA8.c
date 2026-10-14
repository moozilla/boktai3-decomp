#include "modifier_list.h"
#define ModifierMenu_UpdatePreviewAppearance sub_08041DA8
extern u16 gUnk_03004FC0[];
void sub_0816381C(u16 *, u8 *);
void sub_0811D16C(void *);
void sub_0811CCE0(void *, s32);
void ModifierMenu_UpdatePreviewAppearance(struct ModifierMenu *menu)
{
    sub_0816381C((u16 *)((u8 *)menu + 0x130), (u8 *)menu + 0x117);
    if (menu->state != 1) {
        sub_0811D16C((u8 *)menu + 0x3C);
        sub_0811CCE0((u8 *)menu + 0xE34, 0);
    } else {
        u16 *palette = gUnk_03004FC0;
        u32 color = 0x73FF;
        u16 *p;
        s32 i;
        p = &palette[17];
        i = 6;
        do {
            *p++ = color;
        } while (--i >= 0);
        {
            u32 finalColor = 0x73FF;
            palette[25] = finalColor;
        }
    }
}
