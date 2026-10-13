#include "global.h"
struct TradeMenu {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
};
extern void *gUnk_02000580;
void sub_0815C99C(void *);
void sub_08038688(struct TradeMenu *p)
{
    if (p->entered) p->entered = 0;
    if (p->timer > 1) {
        u32 two = 2;
        u8 *state = &p->state;
        u32 zero = 0;
        *state = two;
        p->entered = 1;
        p->timer = zero;
        sub_0815C99C(gUnk_02000580);
    }
    p->timer++;
}
