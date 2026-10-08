#include "global.h"

struct P { u8 f00[0xA4C]; u16 h; u8 fa4e[2]; u32 w; };

void sub_08163EA0(struct P *p, u32 w)
{
    p->w = w;
    p->h = 0;
}
