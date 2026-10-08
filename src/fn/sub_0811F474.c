#include "global.h"
struct S { u8 f[0x10]; u8 a; u8 b; u8 g[2]; u16 c; };
u8 sub_0811F474(struct S *s)
{
    if (s->a && s->c == 0)
        return TRUE;
    return FALSE;
}
