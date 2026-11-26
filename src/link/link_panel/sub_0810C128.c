#include "global.h"
s32 Anim_Step(void *, void *);

bool32 sub_0810C128(u8 *p, void *q)
{
    s32 r = Anim_Step(p, q);
    if ((p[0x1d] & 1) || r)
        return TRUE;
    return FALSE;
}
