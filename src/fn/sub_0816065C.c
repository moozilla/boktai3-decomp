#include "global.h"
#include "render.h"

struct P {
    u8 f00[0x1e];
    u16 h1e;
    u8 f20[4];
    u32 w24;
};
struct G {
    u8 f00[0x540];
    u32 w;
};

void PlayerEffect_SetPaletteSource(struct P *p, u32 h, u32 w)
{
    if (p != 0) {
        p->h1e = h;
        if (w == 0)
            p->w24 = (u32) & (*(struct G **)&gPaletteBankData)->w;
        else
            p->w24 = w;
    }
}
