#include "global.h"

struct Q {
    u8 f[0x1a4];
    u32 a;
};

void MenuCursor_OnHidden(struct Q *p)
{
    u32 m = 1;
    p->a |= m;
}
