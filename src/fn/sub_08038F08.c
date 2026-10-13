// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Menu {
    u8 pad0[0x24];
    u32 timer;
    u8 state, entered;
    u8 pad2a[11];
    u8 retries, result;
    u8 pad37[0x99];
    int window;
    u8 padd4[8];
    int delta;
};
struct Save {
    u8 pad0[12];
    int score;
};
extern struct Save *gUnk_030053F8;
int sub_0822BB7C(void);
void sub_08033924(int, int);
void sub_0822B2F8(int);
void sub_08038F08(struct Menu *p)
{
    int *window = &p->window;
    if (p->entered) {
        struct Save **save;
        int score;
        int delta;
        p->entered = 0;
        save = &gUnk_030053F8;
        score = (*save)->score;
        delta = p->delta;
        score += delta;
        {
            register int maximum asm("r3") = 9999;
            if (score > maximum)
                score = maximum;
            else if (score < 0)
                score = 0;
            (*save)->score = score;
        }
    }
    if (p->timer == 60) {
        p->result = sub_0822BB7C();
        if (p->result == 1) {
            sub_08033924(*window, 14);
            sub_0822B2F8(686);
        } else {
            sub_08033924(*window, 15);
            sub_0822B2F8(402);
        }
    }
    if (p->timer == 240) {
        if (p->result == 1 || ++p->retries > 2) {
            {
                int state = 14;
                register u8 *dest asm("r0") = &p->state;
                int zero = 0;
                *dest = state;
                p->entered = 1;
                p->timer = zero;
            }
        } else {
            sub_08033924(*window, 11);
            {
                int state = 13;
                u8 *dest = &p->state;
                *dest = state;
                p->entered = 1;
                p->timer = 0;
            }
        }
    } else
        p->timer++;
}
