#include "global.h"
struct S {
    u8 p[0x10];
    u32 w10;
};
void ProjectileConfig_SetUpdateCallback(struct S *p, u32 a)
{
    p->w10 = a;
}
