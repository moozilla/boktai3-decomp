#include "global.h"
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0xC25];
    void (*callbackCC8)(struct ChildGroup *);
    u8 padCCC[8];
    void (*callbackCD4)(struct ChildGroup *);
    u8 padCD8[0x18];
    void (*callbackCF0)(struct ChildGroup *);
    u32 timer;
};
void sub_080F1FE8(struct ChildGroup *);
void Ivy_UpdateBobbing(struct ChildGroup *);
void Ivy_WaitForRetarget(struct ChildGroup *);
void Callback_InvokeActor(struct ChildGroup *, void (*)(struct ChildGroup *));
static inline u8 EnterState(struct ChildGroup *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}
void ChildGroup_BeginInactiveState(struct ChildGroup *p)
{
    if (EnterState(p)) {
        p->timer = 100;
        p->callbackCD4 = Ivy_WaitForRetarget;
        p->callbackCF0 = sub_080F1FE8;
    }
    Ivy_UpdateBobbing(p);
    if (p->callbackCD4)
        Callback_InvokeActor(p, p->callbackCD4);
}
