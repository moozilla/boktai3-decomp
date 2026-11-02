#include "homing_projectile.h"

void Memory_Clear(void *, u32);
void HomingProjectile_InitParams(struct HomingParams *p)
{
    Memory_Clear(p, sizeof(*p));
    HomingProjectile_SetBehaviorCallback(p, HomingProjectile_UpdateDefaultState);

    p->unknown0 = 0;
    p->delay = 0x3c;
    p->speed = 5;
    p->variant = p->angle = 0;
    {
        u32 c = 0x1e;
        p->attack = c;
        p->element = 0xf;
        p->attackFlags = c;
        p->shotA = c;
        p->shotB = c;
        p->shotC = c;
    }
}
