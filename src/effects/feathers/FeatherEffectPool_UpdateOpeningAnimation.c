#include "effects/feathers/feather_effect_pool.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"

void FeatherEffectPool_UpdateOpeningAnimation(struct FeatherPool *pool, struct Feather *feather)
{
    struct FeatherSprite *sprite = &feather->sprite;
    u8 row, flip;

    if (feather->enter) {
        feather->sprite.flags &= ~1;
        feather->enter = 0;
    }
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite, &feather->sequence)) {
        Feather_SelectDirection(feather->angle, &row, &flip);
        Anim_SetSequence(&feather->sequence, pool->sequences, 1, row, flip);
        feather->state = 2;
        feather->enter = 1;
    }
}
