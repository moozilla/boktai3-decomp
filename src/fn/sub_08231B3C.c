#include "effect_part.h"

void SceneActorEffects_FinishSlot(void *);

void EffectPart_UpdateBlink(struct EffectPartManager *manager, struct EffectPartGroup *group,
                            struct EffectPart *part)
{
    EffectPart_StepSequence(&part->sprite, manager->graphics, &part->sequence);
    if (part->timer == part->phase) {
        part->sprite.flags |= 1;
        if (part->phase)
            part->phase--;
        part->timer = 0;
    } else
        part->sprite.flags &= ~1;
    if (!part->phase) {
        SceneActorEffects_FinishSlot(part);
        group->active--;
    }
}
