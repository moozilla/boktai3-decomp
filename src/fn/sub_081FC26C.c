#include "global.h"
#define Ratatosk_SpawnAnimatedParticle sub_081FC26C
#include "contact_swarm.h"
#include "display_sequence_inline.h"
#include "ram.h"
struct ParticleConfig {
    u16 appearanceFlags;
    u16 animationId;
    u16 unknown4;
    s8 angle;
    u8 unknown7;
    s32 speed;
    u32 scaleX, scaleZ;
    u32 kind;
    u32 behavior;
};
struct Particle {
    u32 age;
    u8 pad4[8];
    struct ContactPosition velocity;
    struct ParticleConfig config;
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
    struct Particle particles[32];
    void *sequenceResource;
    u32 activeCount;
};

extern void *const gUnk_08613F6C[];
extern const s16 gUnk_086149C4[];
void *Ratatosk_CreateAnimatedParticlePool(void);
struct Particle *Ratatosk_FindFreeAnimatedParticle(struct ParticlePool *, s32 *);
void Ratatosk_SetAnimatedParticleUpdateCallback(void *, void *);
void Sprite_LinkToList(void *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
extern const u16 gUnk_086141BC[];
static inline u32 RandomSample(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline void StepSequence(struct Particle *owner, struct DisplaySequenceState *state,
                                struct ContactSwarmSprite *sprite,
                                struct ContactSwarmSprite **result)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first, second;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        second = (((*command & 0x30) >> 4) & bit);
        *result = sprite;
        if (first != second)
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

// Initialize one particle with a random appearance, animation, and flip state.
s32 Ratatosk_SpawnAnimatedParticle(struct ParticleConfig *config, struct ContactPosition *position)
{
    struct ParticlePool *pool = gRatatoskParticlePool;
    struct Particle *e;
    struct ContactSwarmSprite *sprite;
    s32 slot;
    u32 kind;
    if (!pool) {
        Ratatosk_CreateAnimatedParticlePool();
        return -1;
    }
    e = Ratatosk_FindFreeAnimatedParticle(pool, &slot);
    if (!e)
        return -1;
    Ratatosk_SetAnimatedParticleUpdateCallback(config, gUnk_08613F6C[config->behavior]);
    Memory_Copy(&e->config, config, sizeof(*config));
    e->age = 0;
    e->lift = e->config.scaleZ;
    {
        struct ContactPosition *vel = &e->velocity;
        s32 angle = config->angle;
        s32 speed = config->speed;
        s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        vel->x = x;
        vel->y = 0;
        vel->z = Scale(gUnk_086149C4[angle & 255] * speed);
    }
    e->sprite.position = *position;
    e->config.appearanceFlags = 0;
    kind = RandomSample() & 3;
    if (kind == 3)
        kind = 1;
    e->config.appearanceFlags = kind;
    {
        u16 type = kind;
        // Reject a kind outside the 4-bit field (kind is 0, 1 or 2 here).
        if (type >= 3 && (type & ~15))
            return -1;
    }
    if ((e->config.appearanceFlags & 15) <= 1) {
        if (RandomSample() & 1) {
            Anim_SetSequence(&e->sequence, pool->sequenceResource, 1, 0, 0);
            StepSequence(e, &e->sequence, &e->sprite, &sprite);
        } else {
            struct ContactSwarmSprite *draw;
            u32 frame;
            s32 variant = RandomSample() & 7;
            if (variant > 5)
                variant = 5;
            e->config.appearanceFlags |= variant << 12;
            frame = e->config.appearanceFlags >> 12;
            draw = &e->sprite;
            draw->frame = frame + 3;
            e->config.appearanceFlags |= 0x200;
            sprite = draw;
        }
    } else {
        struct ContactSwarmSprite *draw;
        u32 frame;
        e->config.appearanceFlags |= (RandomSample() & 3) << 12;
        frame = e->config.appearanceFlags >> 12;
        draw = &e->sprite;
        draw->frame = frame + 0x26;
        e->config.appearanceFlags |= 0x200;
        sprite = draw;
    }
    if (RandomSample() & 1) {
        e->sprite.flags |= 4;
        e->sequence.flags |= 1;
        e->config.appearanceFlags |= 0x10;
    }
    if (RandomSample() & 1) {
        e->sprite.flags |= 8;
        e->sequence.flags |= 2;
        e->config.appearanceFlags |= 0x20;
    }
    if (((e->config.appearanceFlags & 15) != 2) && (!(e->config.appearanceFlags & 0x200))) {
        if (RandomSample() & 1) {
            e->sequence.flags |= 4;
            e->config.appearanceFlags |= 0x100;
        }
    }
    e->sprite.flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    e->sprite.priority = 1;
    pool->mask |= 1 << slot;
    pool->activeCount += 1;
    return slot;
}
