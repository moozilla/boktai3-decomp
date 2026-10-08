#include "global.h"

struct S { u8 f[0x1a]; u16 a; u8 g[0xa]; u16 x; u8 h[2]; u16 y; u16 z; };
extern struct S *gUnk_02000114;

void sub_080510B8(void)
{
    struct S *p = gUnk_02000114;
    if (p) {
        p->a = 0;
        p->y = 0;
        p->x = 0;
        p->z = 0;
    }
}
