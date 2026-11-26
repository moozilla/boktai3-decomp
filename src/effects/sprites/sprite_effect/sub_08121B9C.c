#include "engine/video/display_sprite_playback.h"
#include "effects/sprites/sprite_particle_pool.h"

void ContactSequenceSlot_StepPlayback(struct SpriteParticlePool *parent,
                                      struct SpriteParticleSlot *owner)
{
    struct ContactSwarmSprite *sprite;
    struct DisplaySequenceState *state;
    u32 ended;

    sprite = &parent->sprites[owner->index];
    sprite->position = owner->position;
    sprite->flags &= ~1;
    state = (void *)(owner->index * sizeof(*state) + (u32)parent);
    state = ((struct SpriteParticlePool *)state)->animations;
    ended = PlayerCue_StepSequenceEnded(sprite, state);
    if (ended) {
        owner->active = 2;
    }
}
