#include "global.h"
#include "player_cue_sequence.h"
// confidence=low: moves one frost particle, steps playback and hides it on expiry.
#define FrostEmitter_UpdateParticle sub_081D2EA4
struct FrostParticle {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    u16 lifetime, unknown46;
};
static inline void FrostParticle_AddVelocity(struct ContactPosition *out, struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}
void FrostEmitter_UpdateParticle(struct FrostParticle *input)
{
    struct FrostParticle *p = input;
    FrostParticle_AddVelocity(&p->sprite.position, &p->velocity);
    PlayerCue_StepSequence(&p->sprite, &p->sequence);
    if (!--p->lifetime)
        p->sprite.flags |= 1;
}
