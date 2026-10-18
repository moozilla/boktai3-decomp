#include "global.h"

struct S {
    u8 pad[0x3a0];
    u32 f3a0;
};
void Callback_InvokeActor(struct S *, u32);

void FrostEmitter_Update(struct S *p)
{
    if (p->f3a0)
        Callback_InvokeActor(p, p->f3a0);
}
