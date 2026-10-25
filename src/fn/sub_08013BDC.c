#include "global.h"

struct Settings54 {
    u8 pad0[0x54];
    u8 a, b, c, d;
    u16 e, f, g, h;
};
void sub_08013BDC(struct Settings54 *p, int a, int b, int c, int d, int e, int f, int g, int h)
{
    p->a = a;
    p->b = b;
    p->c = c;
    p->d = d;
    p->e = e;
    p->f = f;
    p->g = g;
    p->h = h;
}
