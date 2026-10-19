#include "global.h"
struct VanargandTransition {
    u8 pad0[0xAA];
    u8 state, previousState, phase, subphase, mode, active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
    u8 padC8[0x502 - 0xC8];
    u16 condition;
    u8 pad504[0xF28 - 0x504];
    u32 queued[3];
    s32 queueIndex;
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
void Vanargand_UpdateQueuedActionTransition(void *object)
{
    struct VanargandTransition *p = object;
    struct VanargandTransition *q = object;
    if (Vanargand_ConsumePending(p) && p->previousState != 22) {
        u32 state = 22;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->active) {
        if (q->queueIndex <= 2 && q->queued[q->queueIndex] && !q->condition) {
            u32 phase = 2, mode = 8;
            p->phase = phase;
            p->subphase = 0;
            p->mode = mode;
            p->counter = 0;
            p->pending = 1;
        } else {
            u32 one = 1, zero = 0, mode = 2;
            u32 state;
            p->phase = one;
            p->subphase = zero;
            p->mode = mode;
            p->counter = zero;
            p->pending = one;
            state = 23;
            Vanargand_StopActionSequence((u8 *)p + 0x21C);
            p->state = state;
        }
    } else {
        p->counter++;
    }
}
