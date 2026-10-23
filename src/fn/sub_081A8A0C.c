#include "global.h"

#define Vanargand_UpdateMode23Transition sub_081A8A0C
struct Vanargand {
    u8 pad0[0xAA];
    u8 state, phase, ac, ad, nextState, finished, entered;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
};
void Vanargand_StopActionSequence(void *);
static inline u8 entered(struct Vanargand *p)
{
    u8 *q = &p->entered;
    if (*q) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
void Vanargand_UpdateMode23Transition(struct Vanargand *p)
{
    if (entered(p)) {
        u32 state = 23;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->finished) {
        u32 zero = 0;
        u32 next = 1;
        p->ac = zero;
        p->ad = zero;
        p->nextState = next;
        p->timer = zero;
        p->entered = 1;
    }
    p->timer++;
}
