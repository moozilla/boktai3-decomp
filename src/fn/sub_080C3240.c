#include "global.h"
void Collider_Remove(u8 *);
void sub_0807E908(u8 *);
u32 sub_080C3240(u8 *p)
{
    Collider_Remove(p + 0x624);
    sub_0807E908(p);
    return 0;
}
