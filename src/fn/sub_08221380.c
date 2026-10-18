#define String_CopyPadded sub_08221380
#include "global.h"
s32 sub_08221380(const u8 *source, u8 *dest, s32 length)
{
    s32 i = 0;
    while (i < length && *source) {
        *dest = *source;
        source++;
        dest++;
        i++;
    }
    length++;
    while (i < length) {
        *dest++ = 0;
        i++;
    }
    return 0;
}
