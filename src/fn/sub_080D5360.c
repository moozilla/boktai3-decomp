#include "global.h"

extern const u8 gUnk_08606164[];

struct S {
    u8 filler[648];
    const u8 *p;
};

void Mummy_InitActionHandlers(struct S *s)
{
    s->p = gUnk_08606164;
}
