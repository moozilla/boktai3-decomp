#include "global.h"

void sub_08159860(u32, u32, s32);

void sub_0818F678(u32 a, u32 b, u8 *p) {
    u8 *q = *(u8 **)(p + 0x184);
    *(p + 0x180) = 0xff;
    sub_08159860(a, b, *(s16 *)(q + 0x7274));
}
