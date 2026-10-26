#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x18];
    u32 a;
    u32 b;
};
s32 TargetRegistry_Init(struct S *s)
{
    s->a = 0;
    s->b = 0;
    gSceneLinkList = s;
    return 0;
}
