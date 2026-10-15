#include "stacked_actor.h"
void AnimData_SelectTiles(void *, u32);
void StackedActor_TickDisabled(struct StackedActor *p)
{
    p->sprite.flags |= 1;
    AnimData_SelectTiles(p->graphics, p->tileBase);
}
