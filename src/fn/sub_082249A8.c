#include "global.h"

void sub_08246644(u32);
void sub_082456E0(void);

void sub_082249A8(u8 a) {
    u8 *p = (u8 *)0x03005390;
    u8 old = p[0xe];
    p[0xe] = 1;
    sub_08246644(a);
    sub_082456E0();
    p[0xe] = old;
}
