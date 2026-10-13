#include "global.h"
struct Panel {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
};
struct Panel *sub_08037DC0(void);
void sub_08039898(void)
{
    struct Panel *p = sub_08037DC0();
    if (p) {
        int state = 12;
        u8 *dest = &p->state;
        int zero = 0;
        *dest = state;
        p->entered = 1;
        p->timer = zero;
    }
}
