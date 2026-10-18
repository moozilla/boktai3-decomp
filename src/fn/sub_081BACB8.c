#include "global.h"

struct Q {
    u8 f[0x1a8];
    u32 a;
    u32 b;
};

void SelectionCursor_SetState(struct Q *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
