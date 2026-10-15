#include "stacked_actor.h"
void Sound_PlaySong(u32);
void AnimData_SelectPaletteBank(void *, u32);
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
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase + 2);
    else if (actor->animationPhase)
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase + 1);
    else
        AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase);
    if (actor->lift > 23) {
        StackedActor_SetUpdate(actor, StackedActor_HoldLift);
        Trigger_ConsumePending(actor->trigger, 1);
    }
}
