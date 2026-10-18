#include "global.h"
struct ColorPulse {
    u8 pad0[0xcc4];
    s8 red, green, blue;
    u8 padCC7[0x29];
    void (*callback)(struct ColorPulse *);
    u8 padCF4[8];
    s32 counter;
    u8 padD00[0x32];
    u16 color;
    u8 padD34[0x1e];
    s8 duration;
    u8 padD53, mode;
};
s32 Div(s32, s32);
static inline u16 color(struct ColorPulse *p)
{
    return ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)p->red;
}
void ColorPulse_FinishRedFade(struct ColorPulse *);
s32 ColorPulse_FadeRed(struct ColorPulse *p)
{
    s32 value = Div((u32)p->counter * 8, p->duration) + 16;
    s8 *red = &p->red;
    *red = value;
    if (--p->counter == -1) {
        p->callback = ColorPulse_FinishRedFade;
        p->counter = p->duration;
    }
    p->color = ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)*red;
    return 0;
}
