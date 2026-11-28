#include "effects/feathers/feather_effect_pool.h"
#include "engine/engine.h"
#include "engine/engine_pause.h"
#include "player/cutscene/player_cue_sequence.h"

// confidence=low: creates a directional feather effect with palette and scatter variants.
struct Feather *FeatherEffectPool_CreateDirectional(struct ContactPosition *position, u32 angle,
                                                    u32 flags, u32 variant)
{
    struct FeatherPool *pool = gFadingSpritePool;
    struct Feather *feather;
    struct FeatherSprite *sprite;
    u8 row, flip;

    if (pool == NULL) {
        pool = FeatherEffectPool_GetOrCreate();
        if (pool == NULL)
            return NULL;
    }
    if (Engine_PauseBits(0x400) || Feather_LockBits(4))
        return NULL;
    feather = FeatherEffectPool_AllocEntry(pool);
    if (feather == NULL)
        return NULL;
    feather->age = 0;
    feather->angle = angle;
    feather->enter = 1;
    feather->state = 2;
    sprite = &feather->sprite;
    sprite->position = *position;
    Feather_SelectDirection((u8)angle, &row, &flip);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&feather->sequence, pool->sequences, 1, row, flip);
    feather->variant = variant;
    if (variant == 0) {
        AnimData_SelectPaletteBank(sprite->animData, 0x278);
        feather->effectFlags = 0x200004;
    } else {
        AnimData_SelectPaletteBank(sprite->animData, 0x277);
        feather->effectFlags = 0x400008;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &feather->sequence);
    sprite->flags |= 1;
    sprite->priority = 1;
    sprite->flags |= flags;
    return feather;
}
