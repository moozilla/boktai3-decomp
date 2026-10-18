#include "global.h"

struct P {
    u8 f[0x6c4];
    u32 a;
    u32 b;
    u32 c;
};

void Vorn_InitMotionLimits(struct P *p)
{
    p->a = 0;
    p->b = 0x10000;
    p->c = 0x90000;
}
