#include "global.h"
struct P { u8 f0[0x48]; u16 a; u16 b; u16 c; u8 f1[0x1124 - 0x4E]; u16 x; u16 y; u16 z; };
void sub_08186C98(struct P *p)
{
    p->x = p->a;
    p->y = p->b;
    p->z = p->c;
}
