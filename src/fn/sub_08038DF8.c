#include "global.h"
struct P {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2[0xae];
    u32 title;
};
void sub_08036C38(void);
void sub_08038138(struct P *);
void sub_0821AD08(u32, int);
void sub_08038DF8(struct P *p)
{
    if (p->entered) p->entered = 0;
    if (p->timer > 119) {
        u32 eleven;
        u8 *state;
        u32 zero;
        sub_08036C38();
        eleven = 11;
        state = &p->state;
        zero = 0;
        *state = eleven;
        p->entered = 1;
        p->timer = zero;
        sub_08038138(p);
        if (p->title) sub_0821AD08(p->title, 0);
    } else p->timer++;
}
