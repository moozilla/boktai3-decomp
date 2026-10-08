#include "global.h"

extern u8 *gUnk_03005420;
extern u32 gUnk_030053F4;

void sub_082260A4(s32 a) {
    u8 *s = gUnk_03005420;
    if (s != 0) {
        if ((gUnk_030053F4 & 0x800) == 0) {
            u16 *q = (u16 *)(s + 0x7e);
            if (a > *q)
                *q = a;
        }
    }
}
