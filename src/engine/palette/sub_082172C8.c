#include "engine/shared/global.h"
#include "engine/shared/render.h"

void Palette_ResetObjectAllocation(void)
{
    gObjPaletteBankCount = gObjPaletteReserved + 2;
    gObjPaletteReserved = 0;
}
