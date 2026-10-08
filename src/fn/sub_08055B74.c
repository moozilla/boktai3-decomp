#include "global.h"

struct S { u8 f[0x1bc]; u32 v; };
extern struct S *gUnk_0200011C;

void sub_08055B74(void)
{
    if (gUnk_0200011C)
        gUnk_0200011C->v = 1;
}
