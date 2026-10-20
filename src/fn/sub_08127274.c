// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.8%
// Remaining differences are the cached sprite pointer and animation register allocation.
struct ParticleVector {
    u16 x, y, z, unused;
};
struct ParticleSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Projectile {
    u32 flags;
    u8 unknown4[2], angle, unknown7[9];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    u8 unknown3C[0x1C];
    u8 collider[0x58];
    struct ParticleVector velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
};
struct ProjectilePool {
    u8 unknown0[0x860];
    u32 allocated;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
extern struct TerrainMap *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
extern struct ProjectilePool *gUnk_02000164;
void *TerrainModifiers_FindCellOverride(u32, u32);
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void Sprite_Remove(void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline s32 Cell(struct ParticleVector *p)
{
    s32 x = (s16)p->x >> 8, z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline void Release(struct Projectile *p)
{
    struct ProjectilePool *pool = gUnk_02000164;
    s32 slot = p->slot;
    pool->allocated &= ~(1 << slot);
    p->active = 0;
    Sprite_Remove(p);
    p->slot = -1;
    p->state = 0;
    p->lifetime = 0;
}
static inline u32 Step(struct Projectile *owner, struct ParticleSequence *state,
                       struct Projectile *base)
{
    struct Projectile *sprite = owner;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
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
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
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
                    ended = 1;
                } else {
                    ended = 0;
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
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}
// confidence=low: stationary projectile state expires on animation completion or lifetime.
void SpriteProjectile_UpdateExpiry(void *unused, struct Projectile *input)
{
    struct Projectile *p = input, *sprite = input;
    u32 ended;
    u16 *lifetime;
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    p->cell = Cell(&p->position);
    ended = Step(sprite, &p->sequence, p);
    if (p->colliderFlags & 1) {
        void *collider = p->collider;
        Collider_SetPosition(collider, &p->position, 0);
        Collider_Refresh(collider);
    }
    lifetime = &p->lifetime;
    if (!*lifetime || ended) {
        Release(p);
        p->enter = 1;
    } else
        (*lifetime)--;
}

#else
INCLUDE_ASM("asm/nonmatching", SpriteProjectile_UpdateExpiry);
#endif
