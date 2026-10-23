#include "global.h"

// confidence=low: enters state 25 and requests mode 13 when active.
#define Vanargand_UpdateMode25Transition sub_081A8B28
struct VanargandMode25 {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB;
    u8 clearAC, clearAD, mode, active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
    u8 padC8[0x5C0 - 0xC8];
    u16 phase;
};
void Vanargand_StopActionSequence(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandMode25 *p)
{
    u8 *q = &p->pending;
    if (*q != 0) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void Vanargand_RequestMode(struct VanargandMode25 *p, u32 mode, u32 zero)
{
    p->clearAC = zero;
    p->clearAD = zero;
    p->mode = mode;
    p->counter = zero;
    p->pending = 1;
}
void Vanargand_UpdateMode25Transition(struct VanargandMode25 *p)
{
    struct VanargandMode25 *q = p;
    if (Vanargand_ConsumePending(p)) {
        u32 state = 25;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->active) {
        q->phase = 0;
        Vanargand_RequestMode(p, 13, 0);
    } else {
        p->counter++;
    }
}
