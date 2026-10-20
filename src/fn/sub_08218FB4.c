#include "global.h"

#define TextWindow_DrawWideGlyph sub_08218FB4

u32 Text_GetFontResource(void);
s32 Text_LoadFontResource(void);
u8 *Text_GetWideGlyphData(void);
u16 *Video_GetBackgroundMap(u32);
s32 Text_DrawGlyph(u8 *data, u32 glyph, u32 part, u32 arg, u32 wide);

void TextWindow_DrawWideGlyph(u16 glyph, s32 x, s32 y, u32 arg)
{
    u8 *data;
    u16 *map;
    s32 i, j;

    if (Text_GetFontResource() == 0 && Text_LoadFontResource() < 0)
        return;
    data = Text_GetWideGlyphData() + glyph * 128;
    map = Video_GetBackgroundMap(0);
    for (i = 0; i < 2; i++) {
        u32 rowOff = ((y + i) & 0x1F) << 5;
        for (j = 0; j < 2; j++) {
            s32 tile = Text_DrawGlyph(data, glyph, i * 2 + j, arg, 1);
            if (tile < 0)
                return;
            tile += 0x100;
            data += 0x20;
            *(u16 *)(((((x + j) & 0x1F) + rowOff) * 2) + (u32)map) = tile | ~0xFFF;
        }
    }
}
