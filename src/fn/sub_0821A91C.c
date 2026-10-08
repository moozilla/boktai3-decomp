#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
void sub_0821A91C(s32 n, u32 v) { *(u32 *)((u8 *)gUnk_02000610.sp - (n * 4 + 4)) = v; }
