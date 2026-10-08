#include "global.h"

void sub_082264D0(u32);

void sub_08226508(u32 a) {
    u8 *s = *(u8 **)0x03005420;
    if (s != 0) {
        *(u16 *)(s + 0x1e) = 0;
        sub_082264D0(a);
    }
}
