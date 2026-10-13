#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct P {
    u8 pad[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2[7];
    u8 countdown, retries, ready;
    u8 pad3[14];
    u8 cancel;
    u8 pad4[0xa7];
    u8 wait;
    u8 pad5[5];
    u32 requested;
};
extern u16 gUnk_03005260[];
void sub_08038170(struct P *);
void sub_08038448(struct P *);
void sub_08169DCC(void);
void sub_08169E94(void);
void sub_08038128(struct P *, int);
void sub_08038138(struct P *);
void sub_0822B2F8(int);
static inline u32 test(u16 *flags, u32 mask) { return *flags & mask; }
void sub_08038764(struct P *p)
{
    if (p->entered) {
        u32 zero = 0;
        p->entered = zero;
        sub_08038170(p);
        sub_08038448(p);
        p->requested = zero;
        sub_08169DCC();
        p->ready = 1;
        p->countdown = 8;
        p->wait = 6;
        sub_08038128(p, 2);
    }
    if (gUnk_03005260[1] & 8) {
        p->requested = 1;
        p->countdown = 8;
    }
    if (p->ready && p->timer > 16 && p->requested) {
        register u32 value asm("r0") = p->countdown;
        u32 countdown = value;
        if (countdown == 0) {
            u32 four;
            u8 *state;
            sub_0822B2F8(0xdd);
            four = 4;
            state = &p->state;
            *state = four;
            p->entered = 1;
            p->timer = countdown;
            sub_08169E94();
            sub_08038138(p);
            if (p->cancel && p->retries) p->retries--;
            return;
        }
        p->countdown = value - 1;
    }
    p->timer++;
}
