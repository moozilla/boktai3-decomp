#include "effect_part.h"

void SceneActorEffects_FinishSlot(void *);

void EffectPart_UpdateFinish(struct EffectPartManager *manager, struct EffectPartGroup *group,
                             struct EffectPart *part)
{
    u32 done = EffectPart_StepSequence(&part->sprite, manager->graphics, &part->sequence);
    if (done) {
        SceneActorEffects_FinishSlot(part);
        group->active--;
    }
}
