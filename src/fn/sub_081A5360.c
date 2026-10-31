#include "global.h"
#include "damage_number_display.h"
static inline void SetScale(struct ContactPosition *p, s16 x, s16 y, s16 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
void DamageNumberDisplay_ShowWithDefaultScale(void *a, int b, int c, void *d, int e)
{
    struct ContactPosition scale;
    SetScale(&scale, 32, 0, 32);
    DamageNumberDisplay_Show(a, b, c, d, &scale, e);
}
