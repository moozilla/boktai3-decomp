#include "global.h"
#include "text.h"
/* Text_GetNarrowGlyphCount: Returns the first font header count provisionally interpreted as the narrow glyph count */
#define Text_GetNarrowGlyphCount sub_08218C40


u16 sub_08218C40(void)
{
    if (gFontResource == NULL)
        return 0;
    return gFontResource->narrowGlyphCount;
}
