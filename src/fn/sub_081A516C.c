#include "global.h"

struct T {
    u32 a[3];
};
extern const struct T gUnk_0824F77C;
void Callback_InvokeActor(void *, u32);
struct A {
    u8 f[0x138];
    u16 idx;
};

void Hresvelgr_UpdateMovingProjectile(struct A *p)
{
    struct T t = gUnk_0824F77C;
    Callback_InvokeActor(p, t.a[p->idx]);
}
