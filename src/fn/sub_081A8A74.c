#define Vanargand_UpdateMode20 sub_081A8A74
#include "global.h"
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
void sub_081A8A74(struct Vanargand *p)
{
    if (entered(p)) {
        u32 state = 20;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->finished) {
        u32 zero = 0;
        u32 next = 11;
        p->ac = zero;
        p->ad = zero;
        p->nextState = next;
        p->timer = zero;
        p->entered = 1;
    } else {
        p->timer++;
    }
}
