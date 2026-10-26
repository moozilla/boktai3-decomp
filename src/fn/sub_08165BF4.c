#include "global.h"
#include "render.h"
struct O {
    u8 f0[0x3a];
    u16 a;
    u8 f1[0xc];
    u8 *b;
};
void PauseMenu_SetSpritePalette(struct O *o, u16 v)
{
    o->a = v;
    o->b = gPaletteBankData + (o->a << 5);
}
