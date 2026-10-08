#include "global.h"
struct P { u8 f[0x7298]; u16 a, b, c; u8 g[2]; u16 x, y, z; };
void sub_08195A14(struct P *p)
{
    p->a = p->x;
    p->b = p->y;
    p->c = p->z;
}
