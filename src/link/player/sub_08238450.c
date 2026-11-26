#include "global.h"

struct S08238450 {
    u8 filler[0x41A];
    u16 v;
};

void Player_SetSecondaryActionFlags(struct S08238450 *p, u32 m)
{
    p->v |= m;
}
