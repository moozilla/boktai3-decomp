#include "effect_part.h"

void EffectPart_UpdateLoopAnimation(struct EffectPartManager *manager,
                                    struct EffectPartGroup *group, struct EffectPart *part)
{
    EffectPart_StepSequence(&part->sprite, manager->graphics, &part->sequence);
    {
        u16 *frame = &part->sequence.index;
        if (*frame == 5)
            *frame = 0;
    }
    if (part->lifetime > 0 && --part->lifetime == 0) {
        part->state = 2;
        part->phase = 10;
        part->timer = 0;
        group->count--;
    }
}
