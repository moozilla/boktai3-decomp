#include "stacked_actor.h"
void AnimData_SelectTiles(void *, u32);
void Task_Kill(struct StackedActor *);
void StackedActor_Disappear(struct StackedActor *actor)
{
    if (actor->lift > 2) {
        actor->lift -= 2;
        actor->sprite.flags &= ~1;
        actor->sprite.position.y = (u16)actor->world.y + actor->lift;
    } else
        actor->sprite.flags |= 1;
    if (actor->animationPhase)
        actor->animationPhase--;
    if (actor->animationPhase > 7)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 2);
    else if (actor->animationPhase)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 1);
    else
        AnimData_SelectTiles(actor->graphics, actor->tileBase);
    {
        u16 *frame = &actor->frame;
        u32 value = *frame + 1;
        *frame = value;
        if ((u16)value <= 63) {
            u32 scale;
            actor->sprite.flags |= 2;
            actor->shadow.flags |= 2;
            scale = 64 - *(u8 *)frame;
            actor->sprite.scaleX = scale;
            actor->sprite.scaleY = scale;
            actor->sprite.rotation = *(u8 *)frame << 2;
            actor->shadow.scaleX = actor->sprite.scaleX;
            actor->shadow.scaleY = actor->sprite.scaleX;
            actor->shadow.rotation = actor->sprite.rotation;
        } else {
            actor->sprite.flags |= 1;
            actor->shadow.flags |= 1;
            Task_Kill(actor);
        }
    }
}
