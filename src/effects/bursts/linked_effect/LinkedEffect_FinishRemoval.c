#include "effects/bursts/linked_effect.h"
#include "player/cutscene/player_cue_sequence.h"

// confidence=low: terminal animation releases the linked effect slot and collider.
void LinkedEffect_FinishRemoval(struct LinkedEffectPool *pool, struct LinkedEffect *effect)
{
    if (effect->entered) {
        Anim_SetSequence(&effect->animation, pool->sequences, 24, 0, 0);
        effect->entered = 0;
    }
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&effect->sprite,
                                    &effect->animation)) {
        pool->occupied &= ~(1 << effect->slot);
        effect->state = 0;
        if (effect->sprite.active)
            Sprite_Remove(effect);
        Collider_Remove(&effect->colliders[1]);
        effect->slot = -1;
        effect->unk_174 = 0;
    }
}
