#include "global.h"

struct S {
    u8 f[0x1e];
    u8 a;
};
u32 WirelessGameDisplay_RequestCompletion(struct S *p)
{
    p->a = 1;
    return 0;
}
