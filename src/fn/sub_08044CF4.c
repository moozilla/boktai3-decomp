#include "global.h"
s32 sub_08249274(s32, s32);
s32 sub_08249350(s32, s32);
u32 sub_08044CF4(const u8 *base, const u8 **source, s32 count, s32 index)
{
    const u8 *p;
    s32 value;
    s32 i;
    u32 result;
    result = 0;
    p = *source;
    i = 0;
    if (i < count) {
        do {
            s32 offset;
            s32 shift;
            offset = sub_08249274((s32)p, index);
            shift = sub_08249350((s32)p, index);
            value = base[offset];
            value >>= shift;
            value &= 1;
            value <<= i;
            result |= value;
            p++;
            i++;
        } while (i < count);
    }
    *source = p;
    return result;
}
