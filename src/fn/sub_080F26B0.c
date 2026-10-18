#include "global.h"

u32 Callback_InvokeActor(void *, u32);
void ColorPulse_PrepareTargetDash(void);

struct S {
    u8 filler[0xcd8];
    void (*fp)(void);
    u8 f2[0xcf0 - 0xcdc];
    u32 v;
    u8 f3[0xd53 - 0xcf4];
    s8 b;
};

void Ivy_WaitForPrimaryActionCompletion(struct S *s)
{
    u32 v = s->v;
    if (v != 0) {
        if ((u8)Callback_InvokeActor(s, v) != 0)
            s->b++;
    }
    if (s->b > 1) {
        s->b = 0;
        s->fp = ColorPulse_PrepareTargetDash;
    }
}
