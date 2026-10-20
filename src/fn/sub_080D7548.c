#include "global.h"

void Mummy_RequestStraightRoute(void *);

struct S {
    u8 filler[0x2d4];
    u16 f;
};

void Mummy_OnMovementUpdate(struct S *s)
{
    u32 m = 0x100;
    if (!(s->f & m))
        Mummy_RequestStraightRoute(s);
}
