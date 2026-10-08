#include "global.h"
struct S { u8 f[0x500]; u8 b[0x20]; u8 pd[2]; u16 h; };
extern struct S *gUnk_02000580;
void sub_081C17EC(void)
{
    s32 i;
    for (i = 0; i <= 0x1f; i++) gUnk_02000580->b[i] = 0;
    gUnk_02000580->h = 0;
}
