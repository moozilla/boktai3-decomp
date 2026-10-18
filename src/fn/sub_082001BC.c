#include "global.h"
struct S {
    u8 p[0xc];
    u32 w;
};
void HomingProjectile_SetBehaviorCallback(struct S *p, u32 v)
{
    p->w = v;
}
