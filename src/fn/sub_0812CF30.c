#include "global.h"
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
void sub_0812CF30(u8 *p)
{
    u8 *q = p + 0x3C;
    Collider_SetPosition(q, p + 0x1c, 0);
    Collider_Refresh(q);
}
