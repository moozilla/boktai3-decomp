#include "global.h"

void sub_080414C4(u8 *, u32);

void sub_080414E4(u8 *p) {
    u8 *s = p + 0x90;
    sub_080414C4(p + 0x24, *(s16 *)(s + 0x1c));
    sub_080414C4(p + 0x28, *(s16 *)(s + 0x24));
    sub_080414C4(p + 0x2c, *(s16 *)(s + 0x26));
}
