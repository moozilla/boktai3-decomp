#include "global.h"
s32 __divsi3(s32, s32);
s32 sub_08249350(s32, s32);
void EncodedRecord_WriteBits(u8 *base, s32 bits, u32 *source, s32 count, s32 index)
{
    u32 p;
    s32 i;
    s32 offset;
    s32 shift;
    s32 value;
    u8 *address;
    if (count == 0)
        return;
    if ((u32)bits > (u32)((1 << count) - 1))
        bits = (1 << count) - 1;
    i = 0;
    if (i < count) {
        do {
            p = *source;
            offset = __divsi3((s32)p, index);
            shift = sub_08249350((s32)p, index);
            value = bits >> i;
            value &= 1;
            address = base + offset;
            value <<= shift;
            value |= *address;
            *address = value;
            p++;
            *source = p;
            i++;
        } while (i < count);
    }
}
