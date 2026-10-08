#include "global.h"

void sub_08225F5C(s32 a) {
    u8 *s = *(u8 **)0x03005420;
    if (s != 0) {
        if (a <= 1) {
            *(u16 *)(s + 0x54) = 0;
        } else {
            *(u16 *)(s + 0x56) = a;
            *(u16 *)(s + 0x54) = 3;
        }
    }
}
