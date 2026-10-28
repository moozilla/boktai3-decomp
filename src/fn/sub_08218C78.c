#include "global.h"
#include "text.h"


u32 Text_GetNarrowGlyphData(void)
{
    if (gFontResource == 0)
        return 0;
    return gFontResource->narrowGlyphData;
}
