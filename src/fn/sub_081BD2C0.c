#include "global.h"
struct P {
    u8 f[0x930];
    u32 a;
};
void Callback_InvokeActor(void *, u32);
s32 DarkLoan_OnUpdate(struct P *p)
{
    Callback_InvokeActor(p, p->a);
    return 0;
}
