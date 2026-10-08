#include "global.h"
void sub_08219DD8(void *, u32);
void sub_081FF354(void *, u32, u32, u32, u32);
void sub_081FF368(void *, u32, u32, s32);
void sub_081FF370(void *, void (*)(void));
void sub_081FED9C(void);
struct S { u8 p[0x2c]; u32 w2c; u16 h30; };
void sub_081FF30C(struct S *p)
{
    u32 z;
    sub_08219DD8(p, 0x34);
    sub_081FF354(p, 0, 0x78, 0, 5);
    sub_081FF368(p, 0x848F, 0, -1);
    sub_081FF370(p, sub_081FED9C);
    z = 0;
    p->w2c = z;
    p->h30 = z;
}
