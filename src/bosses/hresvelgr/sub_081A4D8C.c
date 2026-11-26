#include "global.h"

struct E {
    u8 f[0xdc];
    u32 v;
    u8 g[8];
};
struct A {
    u8 f[0x64];
    struct E e[16];
};
void Callback_InvokeActor(void *, u32);

void Hresvelgr_UpdateProjectilePool(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 15; i >= 0; i--) {
        if (e->v != 0)
            Callback_InvokeActor(e, e->v);
        e++;
    }
}
