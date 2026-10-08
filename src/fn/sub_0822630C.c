#include "global.h"

void sub_08226268(u32);

void sub_0822630C(u32 a) {
    u8 *s = *(u8 **)0x03005420;
    if (s != 0) {
        *(u16 *)(s + 0x1e) = 0;
        sub_08226268(a);
    }
}
