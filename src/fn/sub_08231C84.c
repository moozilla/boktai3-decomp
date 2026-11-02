#include "effect_part.h"

void EffectPart_UpdateAnimation2(struct EffectPartManager *manager, struct EffectPartGroup *group,
                                 struct EffectPart *part)
{
    EffectPart_StepSequence(&part->sprite, manager->graphics, &part->sequence);
    if (part->lifetime > 0 && --part->lifetime == 0) {
        part->state = 2;
        part->phase = 10;
        part->timer = 0;
        group->count--;
    }
}
