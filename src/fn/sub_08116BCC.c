#include "global.h"
extern u8 *gUnk_02000710;
struct S { u8 f[0x74]; u32 a; };
extern struct S *gUnk_020004F0;
void sub_08116BCC(void)
{
    if (gUnk_020004F0)
        *(u16 *)(gUnk_02000710 + 0x758) = gUnk_020004F0->a;
}
