#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x18];
    u32 a;
    u32 b;
};
u32 TextWindowManager_Init(struct S *p)
{
    gWindowManager = p;
    p->a = 0;
    p->b = 0;
    return 0;
}
