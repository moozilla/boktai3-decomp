#include "global.h"

struct Task { u32 f0, f4, f8, fC; };

void sub_0821A04C(struct Task *t, u32 a, u32 b)
{
    t->f8 = a;
    t->fC = b;
}
