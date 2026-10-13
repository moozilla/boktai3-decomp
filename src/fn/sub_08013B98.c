#include "global.h"
void AnimData_Load(u8 *);
void sub_08214588(u8 *, u8 *);
u32 sub_08013B98(u8 *p, u32 b, u32 c)
{
    u8 *q = p + 0x38;
    AnimData_Load(q);
    p += 0xc;
    sub_08214588(p, q);
    *(u16 *)(p + 0x10) = c;
}
