#include "monsters/sword/sword_task.h"

void Ivy_UpdateChildBobbing(struct SwordComponent *);
static inline u8 Take(struct SwordComponent *s)
{
    u32 test = 1;
    u8 *p = &s->entryFlags;
    u8 value = *p;
    test &= value;
    if (test) {
        u32 zero;
        s32 mask = -2;
        mask &= value;
        zero = 0;
        *p = mask;
        s->entryReset = zero;
        return 1;
    }
    return 0;
}
void Ivy_BeginChildRandomDelay(struct SwordComponent *);
void IvyChild_UpdateAfterBob(struct SwordComponent *s)
{
    if (Take(s)) {
        s->bobUpdate = Ivy_BeginChildRandomDelay;
    }
    Ivy_UpdateChildBobbing(s);
    {
        void (*fn)(struct SwordComponent *) = s->bobUpdate;
        if (fn)
            Callback_InvokeActor(s, fn);
    }
}
