// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "engine_globals.h"
#ifdef NONMATCHING
// fuzzy: 98.0%
// Remaining: scratch registers in the squared-velocity sum; instruction shapes align.
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
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct TerrainMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 *Properties(u32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return ((struct TerrainMap *)gTerrainMap)->cells + (cell * 4 + 12);
}
static inline s32 Height(u32 cell, struct ParticleVector *p)
{
    u8 *data = Properties(cell);
    u32 shape = *data >> 4;
    s32 height = (*data & 15) << 8;
    switch (shape) {
        case 1:
            height -= (u8)p->z;
            break;
        case 2:
            height -= (u8)p->x;
            break;
    }
    return height;
}
static inline u32 Attributes(u32 cell)
{
    return *(u16 *)(Properties(cell) + 2);
}
static inline void Sum(struct ParticleVector *out, struct ParticleVector *a,
                       struct ParticleVector *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void Release(struct Projectile *p)
{
    struct ProjectilePool *pool = gPooledSpriteManager;
    s32 slot = p->slot;
    pool->allocated &= ~(1 << slot);
    p->active = 0;
    Sprite_Remove(p);
    p->slot = -1;
    p->state = 0;
    p->lifetime = 0;
}
// confidence=low: projectile identity inferred from the pool and collider callbacks.
void SpriteProjectile_UpdateRebounds(void *unused, struct Projectile *input)
{
    struct Projectile *p = input;
    register struct Projectile *sprite asm("sl") = input;
    struct ParticleVector next;
    struct ParticleVector *position, *velocity;
    u32 cell, nextCell, attrs;
    s32 floor, nextFloor;
    u8 *delay;
    u16 *lifetime;
    asm("" : "+r"(sprite));
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    p->cell = Cell(&p->position);
    position = &sprite->position;
    velocity = &p->velocity;
    cell = p->cell;
    Math_Atan2((s16)velocity->x, (s16)velocity->z);
    floor = Height(cell, position);
    if ((s16)position->y > floor)
        velocity->y -= 4;
    Sum(&next, velocity, position);
    if ((s16)next.y < floor) {
        velocity->x = Div((s16)velocity->x * 5, 10);
        velocity->y = -Div((s16)velocity->y * 4, 10);
        velocity->z = Div((s16)velocity->z * 5, 10);
        if (p->flags & 2) {
            s8 *spin = &p->spin;
            s32 value = *spin;
            asm("" : "+r"(value));
            *spin = -value;
        }
        {
            register u8 *source asm("r0") = &p->soundDelay;
            register u32 value asm("r1") = *source;
            asm("" : "+r"(source), "+r"(value));
            delay = source;
            if (!value) {
                register u32 *resource asm("r3");
                register u32 *address asm("r0") = (void *)(source - 5);
                register u32 firstResource asm("r2") = *address;
                asm("" : "+r"(address), "+r"(firstResource));
                {
                    register u32 key asm("r1") = 0xB96C;
                    asm("" : "+r"(key));
                    resource = address;
                    if (firstResource == key && p->sequence.unknownA == 6)
                        Sound_PlaySong(0x3C4);
                    else if (*resource == 0xD45E && p->sequence.unknownA == 6)
                        Sound_PlaySong(0x3B0);
                    {
                        register u32 value asm("r0") = 10;
                        asm("" : "+r"(value));
                        *delay = value;
                    }
                }
            }
        }
        {
            register u16 value asm("r1") = floor;
            register struct ParticleVector *dst asm("r0");
            asm("" : "+r"(value));
            dst = position;
            asm("" : "+r"(dst));
            dst->y = value;
        }
        if ((s16)velocity->x * (s16)velocity->x + (s16)velocity->y * (s16)velocity->y +
                (s16)velocity->z * (s16)velocity->z ==
            0) {
            velocity->x = 0;
            velocity->y = 0;
            velocity->z = 0;
            p->spin = 0;
            p->active = 3;
            p->enter = 1;
            goto stepped;
        }
    } else {
        {
            register struct ParticleVector *dst asm("r3") = position;
            asm("" : "+r"(dst));
            dst->y = next.y;
        }
        delay = &p->soundDelay;
    }
    nextCell = Cell(&next);
    nextFloor = Height(nextCell, &next);
    attrs = Attributes(nextCell);
    if (cell != nextCell && (floor < nextFloor || (Attributes(nextCell) & 3))) {
        velocity->x = -Div((s16)velocity->x * 5, 10);
        velocity->z = -Div((s16)velocity->z * 5, 10);
    }
    if ((nextFloor <= 0 || (attrs & 0x40)) && (s16)next.y <= nextFloor) {
        Release(p);
        lifetime = &p->lifetime;
    } else {
        position->x = position->x + velocity->x;
        position->z = position->z + velocity->z;
    stepped:
        lifetime = &p->lifetime;
    }
    {
        u32 flags;
        {
            register struct Projectile *src asm("r1") = sprite;
            asm("" : "+r"(src));
            flags = src->flags;
        }
        if (flags & 2) {
            register s32 amount asm("r0") = p->spinScale * p->spin;
            register u32 angle asm("r2");
            asm("" : "+r"(amount));
            {
                register struct Projectile *src asm("r2") = sprite;
                asm("" : "+r"(src));
                angle = src->angle;
            }
            amount += angle;
            {
                register struct Projectile *dst asm("r3") = sprite;
                asm("" : "+r"(dst));
                dst->angle = amount;
            }
        }
    }
    {
        struct ParticleSequence *state = &p->sequence;
        struct Projectile *base = p;
        register const volatile u16 *command asm("r5"); /* preserve observed packed-command reads */
        u32 ended, duration, flags, bit;
        {
            register u32 offset asm("r0") = state->index << 1;
            register const volatile u16 *table asm("r1");
            table = base->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        {
            u32 frame = *command >> 6;
            register struct Projectile *dst asm("r1") = sprite;
            asm("" : "+r"(dst));
            dst->frame = frame;
        }
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct Projectile *dst asm("r2") = sprite;
                asm("" : "+r"(dst));
                dst->flags |= 4;
            } else {
                register struct Projectile *dst asm("r3") = sprite;
                asm("" : "+r"(dst));
                dst->flags &= ~4;
            }
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
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register u32 value asm("r0");
                {
                    register struct Projectile *src asm("r1") = sprite;
                    asm("" : "+r"(src));
                    value = src->flags;
                }
                {
                    register u32 mask asm("r1") = 8;
                    asm("" : "+r"(mask));
                    value |= mask;
                }
                {
                    register struct Projectile *dst asm("r2") = sprite;
                    asm("" : "+r"(dst));
                    dst->flags = value;
                }
            } else {
                register struct Projectile *dst asm("r3") = sprite;
                asm("" : "+r"(dst));
                dst->flags &= ~8;
            }
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
                    register u32 offset asm("r0") = state->index << 1;
                    register const volatile u16 *table asm("r1");
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
    }
    if (p->colliderFlags & 1) {
        Collider_SetPosition(p->collider, &p->position, 0);
        Collider_Refresh(p->collider);
    }
    {
        register u8 *dst asm("r2") = delay;
        u32 value;
        asm("" : "+r"(dst));
        value = *dst;
        if (value)
            *dst = value - 1;
    }
    if (*lifetime)
        (*lifetime)--;
    else {
        p->active = 3;
        p->enter = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SpriteProjectile_UpdateRebounds);
#endif
