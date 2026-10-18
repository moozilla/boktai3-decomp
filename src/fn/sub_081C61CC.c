#include "global.h"
struct S {
    u8 f[0x36c];
    u32 a;
};
void TimeResult_UpdateBackground(void *);
void sub_081C61AC(void *);
void Callback_InvokeActor(void *, u32);
void TimeResult_Update(struct S *p)
{
    TimeResult_UpdateBackground(p);
    sub_081C61AC(p);
    if (p->a != 0)
        Callback_InvokeActor(p, p->a);
}
