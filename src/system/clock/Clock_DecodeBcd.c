#include "global.h"
u16 Clock_DecodeBcd(u16 value)
{
    u32 result = value & 0xf;

    result += 10 * ((value >> 4) & 0xf);
    result += 100 * ((value >> 8) & 0xf);
    result += 1000 * ((value >> 12) & 0xf);
    return result;
}
