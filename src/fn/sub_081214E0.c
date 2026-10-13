#include "global.h"
void sub_08121458(void);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
void sub_081214E0(u32 a, u8 *s)
{
    u8 *p, *q;
    sub_08121458();
    p = s + 0x44;
    q = s + 0x1c;
    Collider_SetPosition(p, q, 0);
    Collider_Refresh(p);
}
