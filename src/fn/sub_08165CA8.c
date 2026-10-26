#include "global.h"
#include "render.h"
struct P {
    u8 f0[0xA18];
    u16 buf[16];
    u16 a;
    u16 b;
    u8 c;
    u8 d;
    u8 e;
};
void PauseMenu_StartPaletteTransition(struct P *p, u16 a, u16 b, s32 d)
{
    u16 *src;
    u16 *dst;
    s32 i;
    p->a = a;
    p->b = b;
    src = (u16 *)((u8 *)gPaletteBankData + (p->a << 5));
    dst = p->buf;
    i = 15;
    do {
        *dst = *src;
        src++;
        dst++;
        i--;
    } while (i >= 0);
    p->d = d;
    p->c = 0;
    p->e = 1;
}
