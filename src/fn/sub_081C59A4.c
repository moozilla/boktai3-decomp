#include "global.h"
struct S {
    u8 f[0x30c];
    u32 a;
};
void sub_081C58BC(void *);
void Callback_InvokeActor(void *, u32);
void ChallengeMenu_Update(struct S *p)
{
    sub_081C58BC(p);
    if (p->a != 0)
        Callback_InvokeActor(p, p->a);
}
