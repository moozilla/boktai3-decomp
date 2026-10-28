#include "global.h"
#include "text.h"


u32 Text_GetWideGlyphData(void)
{
    if (gFontResource == 0)
        return 0;
    return gFontResource->wideGlyphData;
}
