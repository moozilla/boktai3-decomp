#include "global.h"

struct S {
    u8 pad[0xf4];
    u32 f4;
};
void sub_081C3EF0(struct S *);
void sub_081C3950(struct S *);
void Callback_InvokeActor(struct S *, u32);

void RegistryF3E4_OnTaskUpdate(struct S *p)
{
    sub_081C3EF0(p);
    if (p->f4) {
        sub_081C3950(p);
        Callback_InvokeActor(p, p->f4);
    }
}
