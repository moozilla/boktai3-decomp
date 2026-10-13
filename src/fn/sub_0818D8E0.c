#include "motion_0818.h"

static inline s32 absolute(s32 x)
{
    if (x < 0)
        x = -x;
    return x;
}
void sub_0818D960(struct Motion0818Object *);
void sub_0818D8E0(struct Motion0818Object *p)
{
    if ((p->parent[0x5c2] & 3) == 0) {
        p->byte1DD--;
        if (absolute(p->byte1DD) > 15)
            p->byte1DD = -15;
    }
    p->rotation += p->byte1DD;
    p->timer--;
    if (p->timer < 0) {
        p->timer = 0;
        if (absolute(p->byte1DD) == 15)
            p->callback = sub_0818D960;
    }
}
