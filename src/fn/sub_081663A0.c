#define MenuDisplay_SetPaletteOverrides sub_081663A0
#include "global.h"

struct MenuDisplay {
    u8 pad0[0x15DA];
    u16 channel1Palette;
    u8 pad15DC[0xC];
    void *channel1PaletteData;
    u8 pad15EC[0x4E];
    u16 channel0Palette;
    u8 pad163C[0xC];
    void *channel0PaletteData;
    u8 pad164C[0xAE];
    u16 channel2Palette;
    u8 pad16FC[0xC];
    void *channel2PaletteData;
    u8 pad170C[0x1C6E];
    u16 channel3Palette;
    u8 pad337C[0xC];
    void *channel3PaletteData;
    u8 pad338C[0x1028];
    u8 overridePalette[1];
};
void Anim_SelectPaletteBank(void *, u32);
void sub_081663A0(struct MenuDisplay *p, s32 a, s32 b, s32 c, s32 d)
{
    if (a == 1) {
        p->channel0Palette = 0xB100;
        p->channel0PaletteData = p->overridePalette;
    } else Anim_SelectPaletteBank((u8 *)p + 0x1600, 0x3AA);
    if (b == 1) {
        p->channel1Palette = 0xB100;
        p->channel1PaletteData = p->overridePalette;
    } else Anim_SelectPaletteBank((u8 *)p + 0x15A0, 0x3AA);
    if (c == 1) {
        p->channel2Palette = 0xB100;
        p->channel2PaletteData = p->overridePalette;
    } else Anim_SelectPaletteBank((u8 *)p + 0x16C0, 0x3AA);
    if (d == 1) {
        p->channel3Palette = 0xB100;
        p->channel3PaletteData = p->overridePalette;
    } else Anim_SelectPaletteBank((u8 *)p + 0x3340, 0x3AA);
}
