#include "global.h"
void Anim_Step(void *, void *);
void sub_0811CCB0(u8 *s)
{
    u8 *p;
    Anim_Step(s + 0xc0, p = s + 0x1e0);
    Anim_Step(s + 0x120, p);
    Anim_Step(s + 0x180, p);
}
