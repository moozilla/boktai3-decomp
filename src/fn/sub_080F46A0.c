#include "global.h"

struct P {
    u8 f[0xcc8];
    void *a;
    u8 g[0xcd0 - 0xccc];
    void *b;
};
void sub_0824923C(void *, void *);
void Ivy_UpdateGroundHeight(void *);

void Ivy_UpdateActions(struct P *p)
{
    if (p->a != 0) {
        sub_0824923C(p, p->a);
        if (p->b != 0)
            sub_0824923C(p, p->b);
        Ivy_UpdateGroundHeight(p);
    }
}
