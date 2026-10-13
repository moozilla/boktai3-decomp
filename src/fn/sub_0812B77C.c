#include "global.h"
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void sub_0812B77C(u8 *p)
{
    u8 *a = p + 0xf4;
    u8 *b = p + 0x1c;
    Collider_SetPosition(a, b, 0);
    Collider_SetPosition(p + 0x148, b, 0);
    Collider_Refresh(a);
}
