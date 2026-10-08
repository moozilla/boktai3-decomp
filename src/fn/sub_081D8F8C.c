#include "global.h"
struct Q { u8 f[0x10]; u16 v; u8 g[0x1C - 0x12]; };
void sub_081DAD0C(void *, void *);
void sub_081D8FAC(void);
s32 sub_081D8F8C(struct Q *p)
{
    sub_081DAD0C((u8 *)p + 0x18, sub_081D8FAC);
    (p + 5)->v = 0x19;
    return 0;
}
