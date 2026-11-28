#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "scene/collision/contact_swarm.h"
#include "player/cutscene/player_cue_sequence.h"
extern const s16 gUnk_086149C4[];
void RatatoskParticle_Remove(struct ParticlePool *, struct Particle *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void ClearFlags(u32 mask, struct Particle *e)
{
    e->motionFlags &= mask;
}
s32 Ratatosk_UpdateParticlePool(struct ParticlePool *pool)
{
    s32 i;
    struct Particle *e;
    s32 last;
    e = pool->particles;
    i = 0;
    do {
        if (pool->mask & (1 << i)) {
            struct ContactSwarmSprite *sprite = &e->sprite;
            struct ParticleConfig *cfg = &e->config;
            if (((s32(*)(struct Particle *, s32))cfg->callback)(e, i) == 1) {
                RatatoskParticle_Remove(pool, e, i);
            } else {
                u32 active = e->motionFlags & 1;
                if (active) {
                    struct ContactPosition *dst = &e->velocity;
                    s32 angle = cfg->angle;
                    s32 speed = cfg->mode;
                    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                    dst->x = x;
                    dst->y = 0;
                    dst->z = Scale(gUnk_086149C4[angle & 255] * speed);
                    ClearFlags(~1, e);
                }
                {
                    struct ContactPosition *pos = &sprite->position;
                    pos->x += e->velocity.x;
                    {
                        s16 *v = &e->velocity.x;
                        pos->y += v[1];
                    }
                    {
                        s16 *v = &e->velocity.x;
                        pos->z += v[2];
                    }
                    sprite->position.y -= e->fallSpeed >> 12;
                }
                PlayerCue_StepSequence(sprite, &e->sequence);
                if (e->age >= cfg->duration)
                    RatatoskParticle_Remove(pool, e, i);
                e->age++;
            }
        }
        i++;
        e++;
        last = i;
    } while (last <= 23);
    return 0;
}
