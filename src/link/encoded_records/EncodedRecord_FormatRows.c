#include "global.h"
extern const u8 gUnk_08605934[];
extern const u8 gUnk_08605940[];

void EncodedRecord_FormatRows(u8 *dst, const u8 *src, const u8 *map, u32 count)
{
    s32 row;
    s32 i;
    s32 j;
    const u8 *glyph = src;

    for (row = 0; row < 4; row++) {
        u8 *out = dst + (row << 8);

        for (i = 0; i < count; i++) {
            s32 code = *glyph++ << 1;

            if (code > 0x59) {
                for (j = 0; j < 8; j++)
                    *out++ = gUnk_08605934[j];
            }
            *out++ = map[code];
            *out++ = map[code + 1];
            if (code > 0x59) {
                for (j = 0; j < 9; j++)
                    *out++ = gUnk_08605940[j];
            }
        }
        *out++ = 0;
    }
}
