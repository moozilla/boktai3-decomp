#include "global.h"

struct S {
    u8 f[0x18];
    u8 a[0x40];
    u32 b;
};
void SceneMessages_RegisterReceiver(u8 *, u32, u32);
u32 sub_08021394(struct S *p, u32 x)
{
    SceneMessages_RegisterReceiver(p->a, x, 7);
    return p->b = 0;
}
