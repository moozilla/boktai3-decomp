// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative link result layout: four scores are capped and packed as nibbles.
struct Entry {
    u8 bytes[4], a, b, flag, kind;
    u32 timer;
};

struct P {
    u8 pad[0x20];
    u32 flags, timer;
    u8 state, entered;
    u8 pad2[14];
    u8 scores[4];
    struct Entry entry;
};

void sub_080397D4(int, int);
int sub_08037FB4(struct P *, int, const u32 *, int);
void sub_08038128(struct P *, int);
void sub_080385E0(struct P *);
void sub_08038634(struct P *);
void sub_08037F5C(struct P *);
void sub_08038138(struct P *);

static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void sub_08038838(struct P *p)
{
    u32 data;
    if (p->entered) {
        struct Entry *e;
        register u32 packed asm("r10");
        register int total asm("r9");
        register int i asm("r12");
        register int shift asm("r8");
        register u8 *q asm("r5");
        p->entered = 0;
        data = 0;
        e = &p->entry;
        packed = 0;
        total = 0;
        i = 0;
        shift = 0;
        q = e->bytes;
        do {
            int value;
            register u32 flag asm("r4") = e->flag;
            if (flag) {
                value = *q;
            } else {
                register int old asm("r0") = *q;
                register u8 *score asm("r1");
                register int n asm("r2");
                asm("" : "+r"(old));
                score = p->scores;
                asm("" : "+r"(score));
                score += i;
                n = *score;
                value = old + n;
                total += n;
                *score = flag;
            }
            e->bytes[i] = 0;
            if (value > 15)
                value = 15;
            packed |= value << shift;
            q++;
            i++;
            shift += 4;
        } while (i <= 3);
        if (total > 0)
            sub_080397D4(3, 1);
        data |= packed;
        sub_08037FB4(p, 2, &data, 1);
        sub_08038128(p, 1);
        sub_080385E0(p);
        sub_08038634(p);
    }
    if (test(&p->flags, 12) == 12) {
        u32 five;
        u8 *state;
        u32 zero;
        sub_08037F5C(p);
        {
            register u32 mask asm("r1") = ~12;
            asm("" : "+r"(mask));
            p->flags &= mask;
        }
        five = 5;
        state = &p->state;
        zero = 0;
        *state = five;
        p->entered = 1;
        p->timer = zero;
        sub_08038138(p);
    } else {
        p->timer++;
    }
}
