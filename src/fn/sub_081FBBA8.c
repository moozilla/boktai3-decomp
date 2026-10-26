// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "ram.h"
struct Particle {
    u32 age;
    u8 pad4[8];
    s16 vx, vy, vz, padE;
    u8 config[0x18];
    struct ContactSwarmSprite sprite;
    u32 flags;
    u32 pad5C;
    u32 lift;
    struct DisplaySequenceState sequence;
};
struct ParticlePool {
    u8 task[0x18];
    u32 mask;
    u8 pad1C[0x1c];
    struct Particle particles[24];
};
struct ParticleConfig {
    u8 pad0[6];
    s8 angle;
    u8 pad7;
    s32 speed;
    u8 pad0C[8];
    u32 kind;
};
extern void *const gUnk_08613F60[];
extern const s16 gUnk_086149C4[];
void *Ratatosk_CreateParticlePool(void);
struct Particle *Ratatosk_FindFreeParticle(struct ParticlePool *, s32 *);
void Ratatosk_SetParticleUpdateCallback(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sprite_LinkToList(void *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline struct ContactSwarmSprite *updatePlayback(struct Particle *eIn,
                                                        struct DisplaySequenceState *state,
                                                        struct ContactSwarmSprite *sprite)
{
    register struct Particle *e asm("r5") = eIn;
    const volatile u16 *command;
    register u32 flags asm("r1");
    u32 bit;
    struct ContactSwarmSprite *result;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = e->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        {
            u32 second = ((*command & 0x30) >> 4) & bit;
            result = sprite;
            if (first != second)
                e->sprite.flags |= 4;
            else
                e->sprite.flags &= ~4;
        }
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
        u32 duration;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                    u32 one = 1;
                    state->duration = one;
                }
            }
        }
    }
    return result;
}

s32 SceneParticles_SpawnAtPosition(struct ParticleConfig *config, struct ContactPosition *position)
{
    struct ParticlePool *pool = gParticlePool;
    register struct Particle *e asm("r5");
    s32 slot;
    u32 zero2;
    if (!pool) {
        Ratatosk_CreateParticlePool();
        return -1;
    }
    e = Ratatosk_FindFreeParticle(pool, &slot);
    if (!e)
        return -1;
    Ratatosk_SetParticleUpdateCallback(config, gUnk_08613F60[config->kind]);
    Memory_Copy(e->config, config, 0x18);
    e->age = 0;
    e->pad5C = 0;
    e->lift = 0;
    {
        s16 *vel = &e->vx;
        s32 angle = config->angle;
        s32 speed = config->speed;
        s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        s32 zero = 0;
        vel[0] = x;
        vel[1] = zero;
        {
            s32 z = Scale(gUnk_086149C4[angle & 255] * speed);
            zero2 = 0;
            vel[2] = z;
        }
    }
    {
        u32 zero = zero2;
        *(struct ContactPosition *)((u8 *)e + 0x48) = *position;
        {
            struct DisplaySequenceState *state;
            struct ContactSwarmSprite *sprite;
            struct ContactSwarmSprite *link;
            if (*(u32 *)((u8 *)e + 0x28) != 2) {
                state = &e->sequence;
                Anim_SetSequence(state, *(void **)((u8 *)pool + 0xb18), 4, 0, zero);
            } else {
                state = &e->sequence;
                Anim_RestartSequence(state, *(void **)((u8 *)pool + 0xb18), 5, 0, zero);
            }
            sprite = (struct ContactSwarmSprite *)((u8 *)e + 0x2c);
            link = updatePlayback(e, state, sprite);
            e->sprite.flags &= ~1;
            Sprite_LinkToList(link, 0);
            {
                register u8 *kind asm("r1") = (u8 *)e + 0x33;
                *kind = 2;
            }
        }
    }
    pool->mask |= 1 << slot;
    *(u32 *)((u8 *)pool + 0xb1c) += 1;
    return slot;
}
