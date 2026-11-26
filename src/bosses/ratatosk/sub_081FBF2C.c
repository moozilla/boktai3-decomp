#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "scene/collision/contact_swarm.h"
#include "engine/video/display_sequence_sprite.h"

extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void updatePlayback(struct DisplaySequenceState *state,
                                  struct DisplaySequenceSprite *spriteInput)
{
    DisplaySequence_StepSprite(spriteInput, state);
}

static inline u32 TestMask(u32 mask, struct RatatoskParticlePool *pool)
{
    struct RatatoskParticlePool *p = pool;
    return p->mask & mask;
}
static inline void ClearFlags(u32 mask, struct RatatoskParticle *e)
{
    e->motionFlags &= mask;
}
// confidence=low: advances the larger particle records and their animation sequences.
s32 Ratatosk_UpdateAnimatedParticlePool(struct RatatoskParticlePool *aIn)
{
    struct RatatoskParticlePool *a = aIn;
    s32 i;
    struct RatatoskParticle *e;
    s32 last;
    e = a->particles;
    i = 0;
    do {
        if (TestMask(1 << i, a)) {
            struct DisplaySequenceSprite *sprite = &e->sprite;
            struct RatatoskParticleConfig *cfg = &e->config;
            if (cfg->callback(e, i) == 1) {
                Ratatosk_ReleaseAnimatedParticle(a, e, i);
            } else {
                u32 active = e->motionFlags & 1;
                struct ContactPosition *velocity = &e->velocity;
                if (active) {
                    struct ContactPosition *dst = &e->velocity;
                    s32 angle = cfg->angle;
                    s32 speed = cfg->speed;
                    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                    dst->x = x;
                    dst->y = 0;
                    dst->z = Scale(gUnk_086149C4[angle & 255] * speed);
                    ClearFlags(~1, e);
                }
                {
                    struct ContactPosition *pos = &sprite->position;
                    sprite->position.x += e->velocity.x;
                    pos->y += velocity->y;
                    pos->z += velocity->z;
                    if (e->fallSpeed < 0) {
                        s32 magnitude = ~e->fallSpeed;
                        magnitude++;
                        sprite->position.y += magnitude >> 12;
                    } else
                        sprite->position.y -= e->fallSpeed >> 12;
                }
                if (!(e->config.appearanceFlags & 0x200)) {
                    if (e->config.appearanceFlags & 0x100)
                        e->sequence.flags |= 4;
                    updatePlayback(&e->sequence, sprite);
                }
                {
                    u32 age = e->age;
                    struct RatatoskParticleConfig *data = cfg;
                    if (age >= data->lifespan)
                        Ratatosk_ReleaseAnimatedParticle(a, e, i);
                }
                e->age++;
            }
        }
        {
            i += 1;
        }
        e++;
        last = i;
    } while (last <= 31);
    return 0;
}
