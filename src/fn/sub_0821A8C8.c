#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
void sub_0821A8C8(u32 *p) { if (p) gUnk_02000610.sp = p; }
