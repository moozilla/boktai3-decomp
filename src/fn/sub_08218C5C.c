#include "global.h"
#include "text.h"


u16 Text_GetWideGlyphCount(void)
{
    if (gFontResource == 0)
        return 0;
    return gFontResource->wideGlyphCount;
}
