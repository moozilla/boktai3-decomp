#include "route_sprite_playback.h"
static inline s32 Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
void RouteSpriteActor_TickPausing(struct RouteSpritePool *pool, struct RouteSpriteActor *actor)
{
    struct ContactSwarmSprite *sprite = &actor->sprite;
    struct DisplaySequenceState *sequence = &actor->sequence;
    u8 *entered = &actor->entered;
    u32 ended;
    if (*entered) {
        Anim_SetSequence(sequence, pool->sequenceResource, 0xE, 0, 0);
        actor->frames = 0;
        *entered = 0;
        actor->angle = 0;
    }
    ended = PlayerCue_StepSequenceEnded(sprite, sequence);
    if (actor->angle >= 0) {
        sprite->position.y =
            actor->savedPosition.y - Truncate12(gUnk_086149C4[((u8)actor->angle + 64) & 255] * 64);
        actor->angle += 4;
    }
    if (actor->frames <= 59)
        actor->frames++;
    else if (ended) {
        actor->state = 4;
        actor->entered = 1;
    }
}
