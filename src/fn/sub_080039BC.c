#include "global.h"

struct S {
    u8 f[0x18];
    u8 a[0x18];
    u8 b;
};
void ColorFade_ApplyCommands(void *);
void sub_08003784(void *);
void sub_08003808(void *);
u32 sub_080039BC(struct S *p)
{
    ColorFade_ApplyCommands(p);
    sub_08003784(&p->a);
    sub_08003784((u8 *)p + 0x30);
    sub_08003808(p);
    return 0;
}
