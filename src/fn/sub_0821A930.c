#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
void sub_0821A930(void) { gUnk_02000610.f8 = gUnk_02000610.stk; }
