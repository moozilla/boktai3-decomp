#include "global.h"
#include "monsters/sword/sword_return_component_layout.h"
s32 Div(s32, s32);
void Sword_UpdateThrustRetraction(struct SwordReturnComponent *);
void Sword_UpdateThrustExtension(struct SwordReturnComponent *p)
{
    p->x = p->startX + Div(p->dx * p->timer, 20);
    p->y = p->startY + Div(p->dy * p->timer, 20);
    p->z = p->startZ + Div(p->dz * p->timer, 20);
    p->timer++;
    if (p->timer > 20)
        p->callback178 = Sword_UpdateThrustRetraction;
}
