#include "global.h"
#include "color_fade_controller.h"

extern struct ColorFadeController *gUnk_02000020;
extern u32 gUnk_03004DA0;
extern u32 gUnk_03004DA8;
extern u16 gUnk_03005214;
extern u16 gUnk_030051C4;
extern u32 gUnk_030051D4;
extern u32 gUnk_03004DAC;
extern u16 gUnk_030051DC;
extern u16 gUnk_03005210;
extern u32 gUnk_030051C8;
void sub_08003760(struct ColorFadeChannel *);

u32 sub_080039F0(struct ColorFadeController *s)
{
    gUnk_02000020 = s;
    sub_08003760(&s->channels[0]);
    sub_08003760(&s->channels[1]);
    gUnk_03004DA0 = 0x40;
    gUnk_03004DA8 = 0x40;
    gUnk_03005214 = 0x1084;
    gUnk_030051C4 = 0;
    gUnk_030051D4 = 0x40;
    gUnk_03004DAC = 0x40;
    gUnk_030051DC = 0x1084;
    gUnk_03005210 = 0;
    return gUnk_030051C8 = 0;
}
