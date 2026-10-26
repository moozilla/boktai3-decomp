#include "global.h"
#include "render.h"

void Mosaic_SetParameters(u16 a, u8 b, u8 c)
{
    gMosaicHalfword = 1;
    gMosaicParam = a;
    gSpriteMosaicAttr = b;
    gMosaicByte = c;
}
