#include "global.h"

extern u16 gUnk_030051D0;
extern u16 gUnk_03004DB0;

void Palette_SetBackdropColor(u16 v)
{
    gUnk_030051D0 = v;
    gUnk_03004DB0 = 1;
}
