#include "global.h"
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_0812C3B0(u8 *p)
{
    u8 *q = p + 0x3c;
    Collider_SetPosition(q, p + 0x1c, 0);
    Collider_Refresh(q);
}
