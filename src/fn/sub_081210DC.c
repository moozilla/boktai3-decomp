#include "global.h"
void sub_0811C9D8(void *);
void sub_081210DC(u8 *s)
{
    u32 v;
    sub_0811C9D8(s + 0x68);
    if (s == 0 || ((v = *(u16 *)(s + 0x8ba)) & 0x14) == 0) {
        (*(u32 *)(s + 0x8b0))++;
    }
}
