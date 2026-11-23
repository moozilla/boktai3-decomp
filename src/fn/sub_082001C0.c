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
        p->attack = 0x1e;
        p->element = 0xf;
        p->attackFlags = 0x1e;
        p->shotA = 0x1e;
        p->shotB = 0x1e;
        p->shotC = 0x1e;
    }
}
