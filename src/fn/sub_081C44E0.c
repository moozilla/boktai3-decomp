#include "global.h"

struct S { u8 pad[0x1c]; s16 v[3]; u8 pad2[0x64-0x22]; void (*f64)(void); s32 f68; };
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C46A0(void);

void sub_081C44E0(struct S *p)
{
    s16 *v = p->v;
    s16 a = 0x15e, b = 0xc4;
    s32 z = 0;
    p->v[0] = a;
    v[1] = b;
    v[2] = z;
    sub_082279A8(0, 6, 4, 4, 4, 0xFFFF, z);
    p->f64 = sub_081C46A0;
    p->f68 = z;
}
