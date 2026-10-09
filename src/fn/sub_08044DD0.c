#include "global.h"
extern const u8 gUnk_08605934[];
extern const u8 gUnk_08605940[];
void sub_08044DD0(u8 *dst, const u8 *src, const u8 *map, u32 count)
{
    s32 row;
    s32 i;
    s32 j;
    s32 code;
    const u8 *map2;
    const u8 *src2;
    u32 count2;
    u8 *out;
    map2 = map;
    count2 = count;
    src2 = src;
    row = 0;
    do {
        out = dst + (row << 8);
        i = 0;
        if (i < count2) {
            do {
                code = *src2++ << 1;
                if (code > 0x59) {
                    j = 0;
                    do {
                        *out = gUnk_08605934[j];
                        out++;
                        j++;
                    } while (j <= 7);
                }
                *out++ = map2[code];
                *out++ = map2[code + 1];
                if (code > 0x59) {
                    j = 0;
                    do {
                        *out = gUnk_08605940[j];
                        out++;
                        j++;
                    } while (j <= 8);
                }
                i++;
            } while (i < count2);
        }
        *out++ = 0;
        row++;
    } while (row <= 3);
}
