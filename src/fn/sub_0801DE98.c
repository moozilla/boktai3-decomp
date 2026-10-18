#include "global.h"

struct S {
    u8 f[0x7a4];
    u32 a;
};
void Callback_InvokeActor(struct S *, u32);
void sub_0801C24C(struct S *);
u32 ResultTutorial_Finish(struct S *p)
{
    Callback_InvokeActor(p, p->a);
    sub_0801C24C(p);
    return 0;
}
