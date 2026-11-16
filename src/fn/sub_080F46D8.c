#include "global.h"

extern void (*gUnk_08606434[])(void);
s32 Callback_InvokeActor(void *, void *);

void Ivy_DispatchState(u8 *p)
{
    u32 m = 0x10;
    if ((p[0x8d] & m) == 0)
        Callback_InvokeActor(p, gUnk_08606434[p[0x92]]);
}
