#include "global.h"
struct Controller {
    u8 pad[24];
    u8 state, first, count, enabled;
    u16 timers[4], periods[4];
    s8 current[4], target[4];
};
void sub_082190E0(int, int, int);
#define PaletteLevels_Decrease sub_0801655C
void PaletteLevels_Decrease(struct Controller *p)
{
    u16 levels = 0;
    int i;
    for (i = 0; i <= 3; i++) {
        levels |= p->current[i] << (i * 4);
        if ((p->enabled >> i) & 1) {
            if (++p->timers[i] >= p->periods[i]) {
                p->timers[i] = 0;
                if (p->current[i] <= p->target[i]) {
                    p->current[i] = p->target[i];
                    p->enabled &= ~(1 << i);
                } else
                    p->current[i]--;
            }
        }
    }
    sub_082190E0(levels, p->first, p->count);
    if (!p->enabled)
        p->state = 0;
}
