#include "effects/feathers/feather_effect_pool.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"

// confidence=low: emits ground scatter on frame three, then releases the completed feather effect.
void FeatherEffectPool_UpdateScatterAnimation(struct FeatherPool *unused, struct Feather *feather)
{
    struct FeatherSprite *sprite = &feather->sprite;
    if (feather->enter) {
        feather->sprite.flags &= ~1;
        feather->enter = 0;
    }
    if (feather->sequence.index == 3 && feather->sequence.elapsed == 0)
        FeatherEffectPool_SpawnGroundScatter(feather, 4);
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite, &feather->sequence)) {
        struct FeatherPool *pool = gFadingSpritePool;

        pool->used &= ~(1 << feather->slot);
        feather->state = 0;
        Sprite_Remove(&feather->sprite);
        feather->slot = -1;
        feather->age = 0;
        feather->enter = 1;
    }
}
