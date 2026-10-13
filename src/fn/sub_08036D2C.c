// MATCHING_CONCESSION: register-pinning
#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
u8 sub_08036CC4(u32);
void sub_08036D2C(const u8 *source, u8 *destination, int count)
{
    int i = 0;
    while (i < count) {
        u32 first = *source;
        u32 code;
        if (!first)
            break;
        first <<= 24;
        asm("" : "+r"(first) : : "memory");
        if ((s32)first >= 0) {
            code = *source++;
        } else {
            {
                int high;
                code = source[0];
                high = source[1] << 8;
                code |= high;
            }
            source += 2;
        }
        destination[i] = sub_08036CC4(code);
        i++;
    }
    while (i < count)
        destination[i++] = 0;
    destination[count] = 0;
}