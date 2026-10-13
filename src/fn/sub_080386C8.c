#include "global.h"
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[0x18];
    u8 flag;
};
extern u32 gUnk_0300523C;
void sub_0801683C(void);
int sub_08037FB4(struct P *, int, const u32 *, int);
void sub_08038128(struct P *, int);
void sub_08038138(struct P *);
void sub_0803A6F8(void);
void sub_08178404(void);
static inline void set(u32 mask) { gUnk_0300523C |= mask; }
void sub_080386C8(struct P *p)
{
    u32 data;
    u32 bits;
    u32 flags;
    if (p->entered) {
        u32 zero = 0;
        p->entered = zero;
        sub_0801683C();
        data = zero;
        sub_08037FB4(p, 1, &data, 1);
        set(4);
        sub_08038128(p, 1);
        p->flag = zero;
        sub_0803A6F8();
    }
    bits = 128;
    flags = p->flags;
    bits &= flags;
    if (bits) {
        u32 eight;
        u8 *state;
        u32 zero;
        sub_08178404();
        eight = 8;
        state = &p->state;
        zero = 0;
        *state = eight;
        p->entered = 1;
        p->timer = zero;
    } else {
        u32 three = 3;
        three &= flags;
        if (three != 3) goto tick;
        p->flags = flags & ~3;
        p->state = three;
        p->entered = 1;
        p->timer = bits;
    }
    sub_08038138(p);
    return;
tick:
    p->timer++;
}
