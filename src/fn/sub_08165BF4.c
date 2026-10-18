#include "global.h"
struct O {
    u8 f0[0x3a];
    u16 a;
    u8 f1[0xc];
    u8 *b;
};
extern u8 *gUnk_030042E4;
void PauseMenu_SetSpritePalette(struct O *o, u16 v)
{
    o->a = v;
    o->b = gUnk_030042E4 + (o->a << 5);
}
