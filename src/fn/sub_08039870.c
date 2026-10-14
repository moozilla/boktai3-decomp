#include "global.h"
struct Panel {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
};
struct Panel *sub_08037DC0(void);
void sub_08039834(void *);
void sub_08039870(void)
{
    struct Panel *p = sub_08037DC0();
    u8 *alias2;
    struct Panel *alias;
    if (p) {
        int one = 1;
        u8 *state = alias2 = &p->state;
        int zero = 0;
        *state = one;
        state++;
        *state = one;
        p->timer = zero;
        alias = p;
        sub_08039834(alias);
    }
}
