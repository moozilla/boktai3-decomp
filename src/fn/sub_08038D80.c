#include "global.h"
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[5];
    u8 mode;
};
extern u32 gUnk_0300523C;
int sub_08038004(struct P *);
int sub_08037FB4(struct P *, int, const u32 *, int);
static inline void set(u32 mask) { gUnk_0300523C |= mask; }
static inline u32 test(u32 *flags, u32 mask) { return *flags & mask; }
void sub_08038D80(struct P *p)
{
    u32 data;
    if (p->entered) {
        p->entered = 0;
        p->mode = sub_08038004(p);
        data = p->mode;
        sub_08037FB4(p, 5, &data, 1);
        set(4);
    }
    if (test(&p->flags, 0x300) == 0x300) {
        u32 ten;
        u8 *state;
        u32 zero;
        p->flags &= ~0x300;
        ten = 10;
        state = &p->state;
        zero = 0;
        *state = ten;
        p->entered = 1;
        p->timer = zero;
    } else p->timer++;
}
