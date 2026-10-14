#include "global.h"
extern u8 gUnk_08E5FA6C[];
/* Count the leading used entries (up to five, 12 bytes each) of a 64-byte record. */
s32 sub_0822D704(u8 *p)
{
    u8 *record = gUnk_08E5FA6C + p[0xd] * 64;
    s32 count = 0;
    s32 i = 0;
    u8 *entry = record + 4;
    do {
        if (*entry == 0)
            break;
        count++;
        entry += 12;
        i++;
    } while (i <= 4);
    return count;
}
