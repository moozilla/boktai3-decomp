#include "global.h"

u32 Text_ExtractDecimalDigit(s32 *value, s32 place)
{
    u32 digit = 0;

    while (*value >= place) {
        digit++;
        *value -= place;
    }
    return digit;
}
