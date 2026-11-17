#include "feather_effect_pool.h"
#include "ram.h"
#include "player_cue_sequence.h"
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
void FeatherEffectPool_UpdateOpeningAnimation(struct FeatherPool *pool, struct Feather *p)
{
    struct ContactSwarmSprite *sprite = &p->sprite;
    u8 row, flip;
    if (p->enter) {
        p->sprite.flags &= ~1;
        p->enter = 0;
    }
    if (PlayerCue_StepSequenceEnded(sprite, &p->sequence)) {
        SelectDirection(&p->angle, &row, &flip);
        Anim_SetSequence(&p->sequence, pool->sequences, 1, row, flip);
        p->state = 2;
        p->enter = 1;
    }
}
