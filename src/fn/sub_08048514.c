#include "global.h"

struct S {
    u8 f[0x44];
    u8 a;
};
extern u32 gUnk_020000F4;
void Anim_Remove(void *);
u32 sub_08048514(struct S *p)
{
    Anim_Remove(&p->a);
    return gUnk_020000F4 = 0;
}
