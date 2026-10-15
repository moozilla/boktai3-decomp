#include "stacked_actor.h"
#define StackedActor_RaiseLift sub_08052660
void Sound_PlaySong(u32);
void AnimData_SelectTiles(void *, u32);
void Trigger_ConsumePending(void *, u32);
void StackedActor_RaiseLift(struct StackedActor *actor)
{
    u16 *frame = &actor->frame;
    if (!*frame) {
        Sound_PlaySong(0x198);
        (*frame)++;
    }
    if (actor->animationPhase <= 7)
        actor->animationPhase++;
    if (actor->lift <= 22)
        actor->lift += 2;
    else
        actor->lift = 24;
    if (!actor->lift)
        actor->sprite.flags |= 1;
    else {
        actor->sprite.flags &= ~1;
        actor->sprite.position.y = (u16)actor->world.y + actor->lift;
    }
    if (actor->animationPhase > 7)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 2);
    else if (actor->animationPhase)
        AnimData_SelectTiles(actor->graphics, actor->tileBase + 1);
    else
        AnimData_SelectTiles(actor->graphics, actor->tileBase);
    if (actor->lift > 23) {
        sub_08052628(actor, sub_08052744);
        Trigger_ConsumePending(actor->trigger, 1);
    }
}
