#include "effects/feathers/feather_effect_pool.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"
void Sprite_Remove(void *);
static inline void SelectDirection(u8 *angle, u8 *row, u8 *flip)
{
    u32 quadrant = ((*angle + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}
static inline u32 Step(struct ContactSwarmSprite *owner, struct DisplaySequenceState *state,
                       struct Feather *base)
{
    return PlayerCue_StepSequenceEnded(owner, &base->sequence);
}

void FeatherEffectPool_SpawnGroundScatter(struct Feather *, s32);
// confidence=low: emits ground scatter on frame three, then releases the completed feather effect.
void FeatherEffectPool_UpdateScatterAnimation(struct FeatherPool *unused, struct Feather *p)
{
    struct ContactSwarmSprite *sprite = &p->sprite;
    if (p->enter) {
        p->sprite.flags &= ~1;
        p->enter = 0;
    }
    if (p->sequence.index == 3 && !p->sequence.elapsed)
        FeatherEffectPool_SpawnGroundScatter(p, 4);
    if (Step(sprite, &p->sequence, p)) {
        struct FeatherPool *pool = gFadingSpritePool;
        pool->used &= ~(1 << p->slot);
        p->state = 0;
        Sprite_Remove(p);
        p->slot = -1;
        p->age = 0;
        p->enter = 1;
    }
}
