#include "global.h"
/* Tentative family name: owner of the active child-record group. */
#define ChildGroup_UpdateVerticalOscillation Ivy_UpdateBobbing
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[0xE];
    u8 reset;
    u8 padA3[0xC25];
    void (*callbackCC8)(struct ChildGroup *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct ChildGroup *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct ChildGroup *);
    u32 timer;
};
void sub_080F1FE8(struct ChildGroup *);
void Ivy_UpdateBobbing(struct ChildGroup *);
void ChildGroup_UpdateLandingState(struct ChildGroup *);
void Ivy_WaitForPrimaryActionCompletion(struct ChildGroup *);
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
static inline void SetState(struct ChildGroup *p, u8 one, u8 id)
{
    p->state92 = one;
    p->state93 = id;
    p->flags |= one;
}
void ChildGroup_UpdateTimedOscillation(struct ChildGroup *p)
{
    if (EnterState(p)) {
        p->timer = 150;
        p->callbackCF0 = sub_080F1FE8;
    }
    Ivy_UpdateBobbing(p);
    if (--p->timer == 0) {
        SetState(p, 1, 4);
        p->callbackCC8 = ChildGroup_UpdateLandingState;
        p->callbackCD8 = Ivy_WaitForPrimaryActionCompletion;
    }
}
