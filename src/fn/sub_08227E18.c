#include "global.h"
#include "render.h"
void Palette_RefreshObjectBanks(void);
void Palette_ClearGrayscale(void)
{
    gGrayscaleControl = 0;
    Palette_RefreshObjectBanks();
}
