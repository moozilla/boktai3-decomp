#include "global.h"
#include "render.h"

void Palette_ResetObjectAllocation(void)
{
    gObjPaletteBankCount = gObjPaletteReserved + 2;
    gObjPaletteReserved = 0;
}
