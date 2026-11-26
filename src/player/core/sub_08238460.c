#include "global.h"

struct S08238460 {
    u8 filler[0x41A];
    u16 v;
};

u16 Player_GetSecondaryActionFlags(struct S08238460 *p, u16 m)
{
    return p->v & m;
}
