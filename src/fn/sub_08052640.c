#include "stacked_actor.h"
void AnimData_SelectPaletteBank(void *, u32);
void StackedActor_TickDisabled(struct StackedActor *p)
{
    p->sprite.flags |= 1;
    AnimData_SelectPaletteBank(p->graphics, p->paletteBase);
}
