#include "global.h"
// MATCHING_CONCESSION: register-pinning
int Text_CopyWithoutTags(u8 *dst, u8 *src, int limit)
{
    int tag = 0, count = 0;
    u32 c;
    while ((c = *src) != 0) {
        {
            register u32 held asm("r3") = c;
            asm("" : "+r"(held));
            c = held;
        }
        if ((s8)c >= 0) {
            if ((u8)c == '<')
                tag = 1;
            else if (tag == 1 && (u8)c == '>')
                tag = 0;
            else if (!tag) {
                *dst++ = *src;
                if (++count >= limit)
                    break;
            }
            src++;
        } else {
            tag = 0;
            *dst = c;
            src++;
            dst++;
            if (++count >= limit)
                break;
            if (!*src)
                break;
            *dst++ = *src++;
            if (++count >= limit)
                break;
        }
    }
    return count;
}
