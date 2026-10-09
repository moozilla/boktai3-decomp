#include "global.h"
void sub_08115A20(u8 *, u32);
void sub_081153B4(void);
void sub_08115A6C(u8 *s, u32 arg) {
    u32 *p = (u32 *)(s + 0x9c);
    u32 mask = 1;
    *p |= mask;
    sub_08115A20(s, arg);
    *(u32 *)(s + 0xf4) = (u32)sub_081153B4;
    s[0xf1] = mask;
}
