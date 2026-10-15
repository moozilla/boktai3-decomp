#include "stacked_actor.h"
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void StackedActor_HoldLift(struct StackedActor *o)
{
    o->lift = 0x18;
    o->animationPhase = 8;
    o->sprite.flags &= ~1;
    o->sprite.position.y = o->world.y + o->lift;
    AnimData_SelectPaletteBank(&o->graphics, o->paletteBase + 2);
}
