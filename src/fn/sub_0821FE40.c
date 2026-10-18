#include "global.h"
struct S {
    u8 a[6];
    u16 h6;
};
void Collider_InsertPriorityList(struct S *);
void Collider_Register(struct S *);
void Collider_Refresh(struct S *p)
{
    if (p->h6 & 0x2000) {
        if ((p->h6 & 4) == 0)
            Collider_InsertPriorityList(p);
    } else
        Collider_Register(p);
}
