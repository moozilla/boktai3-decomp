// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
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
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    struct ParticleVector velocity;
    u8 collider[0x56];
    u8 active;
    s8 slot;
    u16 age;
    u8 enter, angle;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
void *TerrainModifiers_FindCellOverride(u32, u32);
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
static inline void Sum(struct ParticleVector *out, struct ParticleVector *a,
                       struct ParticleVector *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline u32 Step_UpdateBallisticFlight(struct Projectile *owner,
                                             struct ParticleSequence *state,
                                             struct Projectile *base)
{
    register struct Projectile *sprite asm("r6") = owner;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
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

inline static s32 Cell_X(u16 inputX, struct ParticleVector *p, struct ParticleVector **saved)
{
    s32 x = (s16)inputX >> 8;
    s32 z = (s16)p->z >> 8;
    *saved = p;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct TerrainMap *)gTerrainMap)->rows[z] + x;
}

void TimedSprite_UpdateBallisticFlight(void *unused, struct Projectile *input)
{
    struct Projectile *p = input;
    register struct Projectile *sprite asm("r6") = input;
    struct ParticleSequence *sequence;
    struct ParticleVector next;
    struct ParticleVector *position;
    s32 floor;
    s32 nextFloor;
    p->flags &= ~1;
    sequence = &p->sequence;
    sequence->elapsed = 0;
    if (p->age == 3)
        sequence->index = 0;
    else if (p->age == 6)
        sequence->index = 1;
    else
        sequence->index = 2;
    Step_UpdateBallisticFlight(sprite, sequence, p);
    next = sprite->position;
    Sum(&next, &p->velocity, &next);
    p->velocity.y--;
    {
        register struct ParticleVector *current = &sprite->position;
        {
            u32 cell = Cell_X(sprite->position.x, current, &position);
            floor = Height(cell, position);
        }
    }
    nextFloor = Height(Cell(&next), &next);
    sprite->position = next;
    if ((((s16)sprite->position.y) < floor) || (((s16)sprite->position.y) < nextFloor)) {
        sprite->position.x -= p->velocity.x;
        sprite->position.x -= p->velocity.y;
        sprite->position.z -= p->velocity.z;
        p->velocity.x = 0;
        p->velocity.y = 0;
        p->velocity.z = 0;
        p->age = 0;
        p->active = 2;
        p->enter = 1;
    } else {
        void *collider = p->collider;
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
        p->age++;
    }
}
