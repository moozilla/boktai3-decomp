#include "global.h"
void Anim_Remove(void *);
void sub_0811CD08(u8 *p)
{
    s32 i = 4;
    do {
        Anim_Remove(p);
        p += 0x60;
    } while (--i >= 0);
}
