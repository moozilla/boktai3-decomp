#include "global.h"

extern u32 gUnk_03004DA4;
extern u32 gUnk_030051C0;

void Palette_ResetObjectAllocation(void)
{
    gUnk_03004DA4 = gUnk_030051C0 + 2;
    gUnk_030051C0 = 0;
}
