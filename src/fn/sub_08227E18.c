#include "global.h"
extern u32 gUnk_030051E4;
void Palette_RefreshObjectBanks(void);
void Palette_ClearGrayscale(void)
{
    gUnk_030051E4 = 0;
    Palette_RefreshObjectBanks();
}
