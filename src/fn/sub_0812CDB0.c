#include "global.h"

struct S {
    u8 filler[0x18];
    u8 a[0x20];
    u32 c;
};
extern struct S *gUnk_0200018C;
void AnimData_Load(u8 *, u32);

s32 sub_0812CDB0(struct S *s)
{
    s->c = 0;
    AnimData_Load(s->a, 0x0000F422);
    gUnk_0200018C = s;
    return 0;
}
