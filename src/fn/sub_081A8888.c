
#ifdef NONMATCHING
// fuzzy: 95.4%
// foundry draft (worker best 94.3%); what differs is mostly register allocation and instruction order
#define Vanargand_UpdateTimedAlternateTransition sub_081A8888
#include "global.h"
/* Tentative: action state transition, with restart on flag 0x20000. */
struct VanargandTransition {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0xAA-0xA0];
    u8 state;
    u8 previousState, phase, subphase;
    u8 mode, active, pending;
    u8 padB1[0xC4-0xB1];
    u32 counter;
    u8 padC8[0xF24-0xC8];
    u32 alternate;
};
void Vanargand_StopActionSequence(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandTransition *p)
{
    if (p->pending) {
        p->pending = 0;
        p->active = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u32 Vanargand_TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void sub_081A8888(void *object)
{
    struct VanargandTransition *p = object;
    if (Vanargand_ConsumePending(p)) {
        u32 state = 20;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->active && p->counter > 120) {
        u32 state;
        u32 alternate = ((struct VanargandTransition *)object)->alternate;
        if (!alternate) {
            u8 mode = 13;
            p->phase = alternate;
            p->subphase = alternate;
            p->mode = mode;
            p->counter = alternate;
            p->pending = 1;
            state = 1;
        } else {
            u32 zero = 0, mode = 18;
            p->phase = zero;
            p->subphase = zero;
            p->mode = mode;
            p->counter = zero;
            p->pending = 1;
            state = 19;
        }
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    } else {
        p->counter++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A8888);
#endif
