#include "global.h"
void sub_08234B04(void *); void sub_0824923C(void *, u32); extern u32 gUnk_030051E4;
u32 sub_08234ECC(u8 *s) { sub_08234B04(s); sub_0824923C(s, *(u32 *)(s + 0x274)); gUnk_030051E4 = 2; return 0; }
