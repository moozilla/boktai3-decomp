#include "global.h"
struct S { u8 p[4]; u8 b; }; extern struct S *gUnk_03006A50; u16 sub_08244834(u32); void sub_0824490C(void);
void sub_08244510(void) { u32 r = sub_08244834(0x27); if (r == 0) { gUnk_03006A50->b = r; sub_0824490C(); } }
