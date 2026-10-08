#include "global.h"

struct E { u8 d[16]; };
struct S { u8 f[0xf4]; struct E e[1]; };
struct S *sub_08037DC0(void);
struct E *sub_08037DCC(u32 i)
{
    struct S *p = sub_08037DC0();
    if (!p) return 0;
    return &p->e[i];
}
