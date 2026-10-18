#include "global.h"
struct P {
    u8 f[0x7249];
    u8 a, b;
};
void Nidhoggr_SetBehavior(struct P *p, s32 v)
{
    p->a = v;
    p->b = 0;
}
