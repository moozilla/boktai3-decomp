#include "global.h"

struct S { u8 f[0x78]; u8 a; };
extern u32 gUnk_0200047C;
void sub_08020D08(void *);
u32 sub_08020C28(struct S *p)
{
    sub_08020D08(&p->a);
    return gUnk_0200047C = 0;
}
