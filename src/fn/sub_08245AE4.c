#include "global.h"
struct S { u8 p[9]; u8 b; }; extern struct S *gUnk_03006A84; void sub_08245630(u32, u32);
void sub_08245AE4(u8 a, u16 b) { if (b == 0) gUnk_03006A84->b = 1; sub_08245630(a, b); }
