#include "global.h"
void sub_08232A34(void *, void *); extern u32 gUnk_02000098;
u32 sub_08232EA8(u8 *s) { u8 *p = s + 0x20; s32 i = 3; u32 z; do { sub_08232A34(s, p); p += 0x38; i--; } while (i >= 0); z = 0; gUnk_02000098 = z; return 0; }
