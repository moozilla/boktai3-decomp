#include "global.h"
extern u32 gUnk_030053F4, gUnk_030053F0;
void sub_08020CD4(void *, u32, u32);
void sub_08118890(u8 *s)
{
    u32 m = 0x800;
    if (((gUnk_030053F4 | gUnk_030053F0) & m) == 0) {
        sub_08020CD4(s + 0x870, *(u32 *)(s + 0x910), 0xb);
    }
}
