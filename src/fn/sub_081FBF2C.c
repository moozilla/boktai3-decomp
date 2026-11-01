#include "global.h"
#define Ratatosk_UpdateAnimatedParticlePool sub_081FBF2C
#include "contact_swarm.h"
#include "player_cue_sequence.h"
struct Particle {
    u32 age;
    u8 pad4[8];
    s16 vx, vy, vz, padE;
    u8 pad14[2];
    u16 lifespan;
    u8 pad18[2];
    s8 angle;
    u8 pad1B;
    s32 speed;
    u8 pad20[4];
    u32 extra;
    u8 pad28[8];
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
        if (TestMask(1 << i, a)) {
            struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)((u8 *)e + 0x30);
            u8 *cfg = (u8 *)e + 0x14;
            if (sub_08249240(e, (void *)i, *(void **)(cfg + 0x14)) == 1) {
                Ratatosk_ReleaseAnimatedParticle(a, e, i);
            } else {
                u32 active = *(volatile u32 *)&e->flags & 1;
                s16 *vel = &e->vx;
                if (active) {
                    s16 *dst = vel;
                    s32 angle = *(s8 *)(cfg + 6);
                    s32 speed = *(s32 *)(cfg + 8);
                    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                    s32 zero = 0;
                    dst[0] = x;
                    dst[1] = zero;
                    dst[2] = Scale(gUnk_086149C4[angle & 255] * speed);
                    ClearFlags(~1, e);
                }
                {
                    struct ContactPosition *pos = &sprite->position;
                    sprite->position.x += e->vx;
                    {
                        s16 *v = vel;
                        pos->y += v[1];
                    }
                    {
                        s16 *v = vel;
                        pos->z += v[2];
                    }
                    if (e->lift < 0) {
                        s32 magnitude = ~e->lift;
                        magnitude++;
                        sprite->position.y += magnitude >> 12;
                    } else
                        sprite->position.y -= e->lift >> 12;
                }
                if (!(*(u16 *)((u8 *)e + 0x14) & 0x200)) {
                    if (*(u16 *)((u8 *)e + 0x14) & 0x100)
                        e->sequence.flags |= 4;
                    updatePlayback(&e->sequence, sprite);
                }
                {
                    u32 age = e->age;
                    u8 *data = cfg;
                    if (age >= *(u16 *)(data + 2))
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
