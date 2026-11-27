#include "monsters/sword/sword_wave_component.h"
#include "monsters/sword/sword_task.h"
void IvyChild_UpdateAfterBob(struct SwordComponent *);

void Sword_AccelerateReturnTurn(struct SwordComponent *p)
{
    struct SwordTask *parent = p->parent;
    s32 i;
    for (i = 0; i < p->turnSteps; i++) {
        p->sprite.attribute += (u8)p->orbitDirection;
        if (p->sprite.attribute == 128) {
            SwordWaveComponent_SetAction((void *)p, 1, 1);
            p->update = IvyChild_UpdateAfterBob;
            return;
        }
    }
    if (!(parent->ticks & 7)) {
        if (++p->turnSteps > 6)
            p->turnSteps = 7;
    }
}
