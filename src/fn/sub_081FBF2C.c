// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define Ratatosk_UpdateAnimatedParticlePool sub_081FBF2C
#include "contact_swarm.h"
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
    struct ContactSwarmSprite *sprite = spriteInput;
    const volatile u16 *command;
    u32 flags, bit;
    u32 duration;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        first = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & 1))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r2");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed) {
                    register u32 one asm("r0") = 1;
                    state->duration = one;
                }
            }
        }
    }
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
                    register u8 *data asm("r1") = cfg;
                    asm("" : "+r"(data));
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
