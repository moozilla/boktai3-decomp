#include "global.h"
struct S { u8 p[8]; u8 b; }; extern struct S *gUnk_030053F8; u8 sub_0822BC6C(u32); u8 sub_0822BD20(u32);
u32 sub_0822BDB8(void) { u32 b = gUnk_030053F8->b; if (sub_0822BC6C(b)) { if (sub_0822BD20(b)) return 1; } return 0; }
