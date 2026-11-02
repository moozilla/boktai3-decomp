#include "global.h"
#define Ratatosk_UpdateAnimatedParticlePool sub_081FBF2C
#include "contact_swarm.h"
#include "player_cue_sequence.h"
struct RatatoskParticleConfig {
    u16 flags, lifespan;
    u8 unknown4[2];
    s8 angle;
    u8 unknown7;
    s32 speed;
    u8 unknownC[4];
    u32 extra;
    void *callback;
    u8 unknown18[4];
};
struct Particle {
    u32 age;
    u8 pad4[8];
    struct ContactPosition velocity;
    struct RatatoskParticleConfig config;
    struct ContactSwarmSprite sprite;
    u32 flags;
    u8 pad5C[4];
    s32 lift;
    struct DisplaySequenceState sequence;
};
struct ParticlePool {
    u8 task[0x18];
    u32 mask;
    u8 pad1C[0x1c];
    struct Particle particles[32];
};
extern const s16 gUnk_086149C4[];
void Ratatosk_ReleaseAnimatedParticle(void *, void *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void updatePlayback(struct DisplaySequenceState *state,
                                  struct ContactSwarmSprite *spriteInput)
{
    PlayerCue_StepSequence(spriteInput, state);
}

static inline u32 TestMask(u32 mask, struct ParticlePool *pool)
{
    struct ParticlePool *p = pool;
    return p->mask & mask;
}
static inline void ClearFlags(u32 mask, struct Particle *e)
{
    e->flags &= mask;
}
// confidence=low: advances the larger particle records and their animation sequences.
s32 Ratatosk_UpdateAnimatedParticlePool(struct ParticlePool *aIn)
{
    struct ParticlePool *a = aIn;
    s32 i;
    struct Particle *e;
    s32 last;
    e = a->particles;
    i = 0;
    do {
        if (TestMask(1u << i, a)) {
            struct ContactSwarmSprite *sprite = &e->sprite;
            struct RatatoskParticleConfig *cfg = &e->config;
            if (sub_08249240(e, (void *)i, cfg->callback) == 1) {
                Ratatosk_ReleaseAnimatedParticle(a, e, i);
            } else {
                u32 active = e->flags & 1;
                struct ContactPosition *velocity = &e->velocity;
                if (active) {
                    struct ContactPosition *dst = &e->velocity;
                    s32 angle = cfg->angle;
                    s32 speed = cfg->speed;
                    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                    s32 zero = 0;
                    dst->x = x;
                    dst->y = zero;
                    dst->z = Scale(gUnk_086149C4[angle & 255] * speed);
                    ClearFlags(~1, e);
                }
                {
                    struct ContactPosition *pos = &sprite->position;
                    sprite->position.x += e->velocity.x;
                    pos->y += velocity->y;
                    pos->z += velocity->z;
                    if (e->lift < 0) {
                        s32 magnitude = ~e->lift;
                        magnitude++;
                        sprite->position.y += magnitude >> 12;
                    } else
                        sprite->position.y -= e->lift >> 12;
                }
                if (!(e->config.flags & 0x200)) {
                    if (e->config.flags & 0x100)
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
            u32 one = 1;
            i += one;
        }
        e++;
        last = i;
    } while (last <= 31);
    return 0;
}
