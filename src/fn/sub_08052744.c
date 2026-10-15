#include "stacked_actor.h"
#define StackedActor_HoldLift sub_08052744
void AnimData_SelectTiles(void *, u32);
void Sound_PlaySong(u32);
void StackedActor_HoldLift(struct StackedActor *o)
{
    o->lift = 0x18;
    o->animationPhase = 8;
    o->sprite.flags &= ~1;
    o->sprite.position.y = o->world.y + o->lift;
    AnimData_SelectTiles(&o->graphics, o->tileBase + 2);
}
