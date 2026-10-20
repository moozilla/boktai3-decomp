#include "global.h"

void Ivy_UpdateChildBobbing(u8 *);
void Callback_InvokeActor(u8 *, void (*)(u8 *));
static inline u8 Take(u8 *s)
{
    u32 test = 1;
    u8 *p = s + 0x119;
    u8 value = *p;
    test &= value;
    if (test) {
        u32 zero;
        s32 mask = -2;
        mask &= value;
        zero = 0;
        *p = mask;
        s[0x12E] = zero;
        return 1;
    }
    return 0;
}
void Ivy_BeginChildRandomDelay(u8 *);
#define IvyChild_UpdateAfterBob sub_080F60B8
void IvyChild_UpdateAfterBob(u8 *s)
{
    if (Take(s)) {
        void (**p)(u8 *) = (void (**)(u8 *))(s + 0x170);
        void (*fn)(u8 *) = Ivy_BeginChildRandomDelay;
        *p = fn;
    }
    Ivy_UpdateChildBobbing(s);
    {
        void (*fn)(u8 *) = *(void (**)(u8 *))(s + 0x170);
        if (fn)
            Callback_InvokeActor(s, fn);
    }
}
