#include "global.h"

void sub_0816381C(u16 *out, u8 *c)
{
    u32 v = *c;
    u32 r = 0x1f;
    if (v > 9) {
        r = 0x1b;
        if (v > 0x11) {
            r = 0x12;
            if (v > 0x19) {
                r = 0xa;
                if (v > 0x21) {
                    r = 0x1b;
                    if (v <= 0x29)
                        r = 0x12;
                }
            }
        }
    }
    *c = *c + 1;
    if (*c > 0x31)
        *c = 0;
    *out = r;
}
