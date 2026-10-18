#include "global.h"

struct S {
    u8 f[0xfb];
    u8 a;
};
extern struct S *gUnk_02000484;
void Wireless_RequestInputResync(void)
{
    struct S *p = gUnk_02000484;
    if (p)
        p->a = 1;
}
