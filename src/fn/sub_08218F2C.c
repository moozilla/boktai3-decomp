#include "global.h"

u32 Text_GetFontResource(void);
s32 Text_LoadFontResource(void);
u8 *Text_GetNarrowGlyphData(void);
u16 *Video_GetBackgroundMap(u32);
s32 Text_DrawGlyph(u8 *data, u32 glyph, u32 part, u32 arg, u32 wide);

void TextWindow_DrawNarrowGlyph(u16 glyph, s32 x, s32 y, u32 arg)
{
    u8 *data;
    u16 *map;
    s32 i;

    if (Text_GetFontResource() == 0 && Text_LoadFontResource() < 0)
        return;
    data = Text_GetNarrowGlyphData() + glyph * 64;
    map = Video_GetBackgroundMap(0);
    for (i = 0; i < 2; i++) {
        s32 tile = Text_DrawGlyph(data, glyph, i, arg, 0);
        if (tile < 0)
            return;
        tile += 0x100;
        data += 0x20;
        map[y * 32 + x] = tile | ~0xFFF;
        y = (y + 1) & 0x1F;
    }
}
