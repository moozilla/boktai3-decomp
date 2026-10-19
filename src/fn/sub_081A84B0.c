
#ifdef NONMATCHING
// fuzzy: 91.5%
// foundry draft (worker best 88.2%); what differs is mostly register allocation and instruction order
#define Vanargand_UpdateMode3Transition sub_081A84B0
#include "global.h"
struct VanargandTarget {
    u8 pad0[0x418];
    u8 mode;
};
struct VanargandTransition {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[3];
    u8 mode, active, pending;
    u8 padB1[0xC4-0xB1];
    u32 counter;
    u8 padC8[0x10C-0xC8];
    struct VanargandTarget *target;
    u8 pad110[0x230-0x110];
    u8 targetMode;
    u8 pad231[0x5C2-0x231];
    s16 condition;
};
void Vanargand_StopActionSequence(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandTransition *p)
{
    if (p->pending) {
        p->pending = 0;
        p->active = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u8 Vanargand_IsTargetMode(u8 mode)
{
    if (mode == 0 || mode == 2) return TRUE;
    return FALSE;
}
static inline u8 Vanargand_SelectMode(s16 condition)
{
    if (condition == 0) return 14;
    return 11;
}
void sub_081A84B0(void *object)
{
    struct VanargandTransition *p = object;
    u32 consumed = Vanargand_ConsumePending(p);
    if (consumed) {
        u32 state;
        u8 mode = p->target->mode;
        ((struct VanargandTransition *)object)->targetMode = Vanargand_IsTargetMode(mode);
        state = 3;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    } else if (p->active) {
        if (((struct VanargandTransition *)object)->condition == 0)
            p->mode = 14;
        else
            p->mode = 11;
        p->counter = consumed;
        p->pending = 1;
    }
    p->counter++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A84B0);
#endif
