#include "global.h"

extern u16 gUnk_030042D8;
extern u16 gUnk_030042DC;
extern u16 gUnk_030042E8;
extern u32 gUnk_030042EC;
extern u16 gUnk_030042F4;

void sub_082152AC(void)
{
    gUnk_030042D8 = 0;
    gUnk_030042DC = 0;
    gUnk_030042E8 = 0;
    if (gUnk_030042EC != 0)
        gUnk_030042E8 = gUnk_030042F4;
}
