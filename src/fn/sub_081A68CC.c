#include "global.h"

struct A { u8 f[0x15]; u8 a; u16 b; };
void sub_0822B358(s32);

void sub_081A68CC(struct A *p)
{
    p->b = 0;
    p->a = 0;
    sub_0822B358(0x583);
}
