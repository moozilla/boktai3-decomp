#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
u8 sub_08036CC4(u16);
void sub_08036CF0(const u8 *source, u8 *destination, int count)
{
    int i = 0;
    while (i < count) {
        u16 code = sub_08036CAC(source[i]);
        if (code <= 127) {
            *destination++ = code;
        } else {
            destination[0] = code;
            destination[1] = code >> 8;
            destination += 2;
        }
        i++;
    }
    *destination = 0;
}