#include "global.h"
struct S {
    u8 f[0x5b8];
    u32 a;
};
void Callback_InvokeActor(void *, u32);
void sub_081C18E8(void *);
void sub_081C2240(void *);
u32 ControlPanel_OnTaskUpdate(struct S *p)
{
    Callback_InvokeActor(p, p->a);
    sub_081C18E8(p);
    sub_081C2240(p);
    return 0;
}
