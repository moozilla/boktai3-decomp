#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: random-source lifetimes, sprite pointers and inlined sequence allocation.
#define Ratatosk_SpawnAnimatedParticle sub_081FC26C
#include "contact_swarm.h"
struct Particle {
    u32 age;
    u8 pad4[8];
    s16 vx, vy, vz, padE;
    u16 appearanceFlags;
    u8 config[0x1a];
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
};
struct ParticleConfig {
    u8 pad0[6];
    s8 angle;
    u8 pad7;
    s32 speed;
    u8 pad0C[8];
    u32 kind;
    u32 behavior;
};
extern struct ParticlePool *gUnk_020005E4;
extern void *const gUnk_08613F6C[];
extern const s16 gUnk_086149C4[];
void *Ratatosk_CreateAnimatedParticlePool(void);
struct Particle *Ratatosk_FindFreeAnimatedParticle(struct ParticlePool *, s32 *);
void Ratatosk_SetAnimatedParticleUpdateCallback(void *, void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sprite_LinkToList(void *, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void updatePlayback(struct Particle *eIn, struct DisplaySequenceState *state,
                                  struct ContactSwarmSprite *sprite)
{
    struct Particle *e = eIn;
    const volatile u16 *command;
    u32 flags;
    u32 bit;
    struct ContactSwarmSprite *result;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = e->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        {
            u32 second = (((*command) & 0x30) >> 4) & bit;
            result = sprite;
            if (first != second)
                e->sprite.flags |= 4;
            else
                e->sprite.flags &= ~4;
        }
    }
    {
        u32 input = state->flags;
        u32 bit;
        u32 first;
        bit = 2;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
        first = (u8)first;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 duration;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed)) {
                    u32 one = 1;
                    state->duration = one;
                }
            }
        }
    }
}

extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
static inline u32 RandomSample(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
// confidence=low: randomizes a larger particle record and its sprite or sequence.
inline static void updatePlayback_081FC26C(struct Particle *eIn, struct DisplaySequenceState *state,
                                           struct ContactSwarmSprite *sprite)
{
    struct Particle *e = eIn;
    const volatile u16 *command;
    u32 flags;
    u32 bit;
    struct ContactSwarmSprite *result;
    typeof(state->speed) t1;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = e->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        {
            u32 second = (((*command) & 0x30) >> 4) & bit;
            result = sprite;
            if (first != second)
                e->sprite.flags |= 4;
            else
                e->sprite.flags &= ~4;
        }
    }
    {
        u32 input = state->flags;
        u32 bit;
        u32 first;
        bit = 2;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
        first = (u8)first;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 duration;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed;
                t1 = state->speed;
                speed = t1;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed)) {
                    u32 one = 1;
                    state->duration = one;
                }
            }
        }
    }
}

s32 Ratatosk_SpawnAnimatedParticle(struct ParticleConfig *config, struct ContactPosition *position)
{
    struct ParticlePool *pool = gUnk_020005E4;
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
    Memory_Copy(&e->appearanceFlags, config, 0x1c);
    e->age = 0;
    e->lift = *((u32 *)(((u8 *)e) + 0x24));
    {
        s16 *vel = &e->vx;
        s32 angle = config->angle;
        s32 speed = config->speed;
        s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        s32 zero = 0;
        vel[0] = x;
        vel[1] = zero;
        vel[2] = Scale(gUnk_086149C4[angle & 255] * speed);
    }
    e->sprite.position = *position;
    e->appearanceFlags = 0;
    kind = (kind == 3) ? (1) : (RandomSample() & 3);
    {
        typeof(e->appearanceFlags) *q1 = &e->appearanceFlags;
        *q1 = kind;
    }
    if ((e->appearanceFlags & 15) <= 1) {
        if (RandomSample() & 1) {
            Anim_SetSequence(&e->sequence, *((void **)(((u8 *)pool) + 0xf38)), 1, 0, 0);
            sprite = &e->sprite;
            updatePlayback_081FC26C(e, &e->sequence, sprite);
        } else {
            u32 variant = RandomSample() & 7;
            if (variant > 5)
                variant = 5;
            e->appearanceFlags |= variant << 12;
            sprite = &e->sprite;
            sprite->frame = (e->appearanceFlags >> 12) + 3;
            e->appearanceFlags |= 0x200;
        }
    } else {
        e->appearanceFlags |= (RandomSample() & 3) << 12;
        sprite = &e->sprite;
        sprite->frame = (e->appearanceFlags >> 12) + 0x26;
        e->appearanceFlags |= 0x200;
    }
    if (RandomSample() & 1) {
        e->sprite.flags |= 4;
        e->sequence.flags |= 1;
        e->appearanceFlags |= 0x10;
    }
    if (RandomSample() & 1) {
        e->sprite.flags |= 8;
        e->sequence.flags |= 2;
        e->appearanceFlags |= 0x20;
    }
    if (((e->appearanceFlags & 15) != 2) && (!(e->appearanceFlags & 0x200))) {
        if (RandomSample() & 1) {
            e->sequence.flags |= 4;
            e->appearanceFlags |= 0x100;
        }
    }
    e->sprite.flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    *(((u8 *)e) + 0x37) = 1;
    pool->mask |= 1 << slot;
    *((u32 *)(((u8 *)pool) + 0xf3c)) += 1;
    return slot;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081FC26C);
#endif
