#include "global.h"
/* Text_GetNarrowGlyphCount: Returns the first font header count provisionally interpreted as the narrow glyph count */
#define Text_GetNarrowGlyphCount sub_08218C40

extern u16 *gUnk_03001664;

u16 sub_08218C40(void)
{
    if (gUnk_03001664 == NULL)
        return 0;
    return *gUnk_03001664;
}
