#include "global.h"
void Anim_Step(void *, void *);
void sub_0816BC2C(u8 *s)
{
    u8 *p = s + 0x38E0;
    s32 i;
    for (i = 3; i >= 0; i--) {
        Anim_Step(p, s + 0x84);
        p += 0x60;
    }
}
