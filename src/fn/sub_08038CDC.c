#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Entry {u8 bytes[4], a, b, flag, kind;u16 timer, pad;};
struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[0x12];
    struct Entry entry;
};
extern u32 gUnk_02000560;
extern void *gUnk_02000580;
void sub_0801683C(void);
void sub_08038094(struct P *);
int sub_08037FB4(struct P *, int, const u32 *, int);
void sub_08038128(struct P *, int);
void sub_0822B058(int);
void sub_0815AF14(void *, int);
static inline u32 test(u32 *flags, u32 mask) { return *flags & mask; }
void sub_08038CDC(struct P *p)
{
    u32 data;
    if (p->entered) {
        p->entered = 0;
        sub_0801683C();
        sub_08038094(p);
        {register struct Entry *e asm("r0") = &p->entry;
        data = e->timer | ((e->kind & 3) << 16);}
        sub_08037FB4(p, 4, &data, 1);
        sub_08038128(p, 5);
        sub_0822B058(4);
        {register u32 mask asm("r1") = 4;
        if (!(gUnk_02000560 & mask)) sub_0815AF14(gUnk_02000580, -1);
        }
    }
    if (p->timer > 29 && test(&p->flags, 0xc0) == 0xc0) {
        u32 nine;
        u8 *state;
        u32 zero;
        p->flags &= ~0xc0;
        nine = 9;
        state = &p->state;
        zero = 0;
        *state = nine;
        p->entered = 1;
        p->timer = zero;
    } else p->timer++;
}
