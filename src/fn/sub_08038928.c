#include "global.h"
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[2];
    u8 ready;
    u8 pad3[6];
    u8 resend;
};
int sub_08037FB4(struct P *, int, const u32 *, int);
static inline u32 test(u32 *flags, u32 mask) { return *flags & mask; }
void sub_08038928(struct P *p)
{
    u32 data;
    u32 retry;
    if (p->entered) {
        p->entered = 0;
        p->ready = 0;
        data = 0;
        sub_08037FB4(p, 3, &data, 1);
    }
    if (p->ready && p->resend) {
        u32 zero = 0;
        retry = zero;
        sub_08037FB4(p, 3, &retry, 1);
        p->resend = zero;
    }
    if (test(&p->flags, 0x30) == 0x30) {
        u32 six;
        u8 *state;
        u32 zero;
        p->flags &= ~0x30;
        six = 6;
        state = &p->state;
        zero = 0;
        *state = six;
        p->entered = 1;
        p->timer = zero;
    } else p->timer++;
}
