#include "global.h"
void sub_08237280(void *); extern void *gUnk_020004A0;
u32 sub_082372F8(u8 *s) { sub_08237280(s); *(u16 *)(s + 0x198) = 0; gUnk_020004A0 = s; return 0; }
