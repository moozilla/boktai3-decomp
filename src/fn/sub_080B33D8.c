#include "global.h"

struct Q {
    u8 f[0x6e8];
    u16 v;
};
struct P {
    u8 f[0x3d0];
    struct Q *q;
};

void Vorn_UpdateRecoveryCooldown(struct P *p)
{
    u16 *v = &p->q->v;
    if (*v != 0)
        *v = *v - 1;
}
