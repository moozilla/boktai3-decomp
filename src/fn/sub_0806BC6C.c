#include "global.h"
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void sub_0806BC6C(u8 *p)
{
    u8 *q = p + 0xa8;
    Collider_SetPosition(q, p + 0x1c, 0);
    Collider_Refresh(q);
}
