#include "global.h"

struct S {
    u8 pad[0x25];
    u8 b25;
};

void BestiaryDisplay_Hide(struct S *p)
{
    p->b25 = 1;
}
