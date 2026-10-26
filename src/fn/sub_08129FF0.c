#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 90.0%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Pointer lifetimes, saved registers and terrain-height comparison still differ.
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 pad24[0x24];
};
struct Anim {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
static inline u32 Step(struct Sprite *sprite, struct Sprite *sprite2, struct Anim *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = *(const volatile u16 *const *)((u8 *)sprite + 0x2C);
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
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
            sprite2->flags |= 8;
        else
            sprite2->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */

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

struct Disc {
    u32 flags;
    u8 visible;
    u8 unknown5[3];
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 pad24[8];
    struct Anim anim;
    u8 pad3C[12];
    struct Vec projected;
    u8 pad50[0x34];
    struct Vec target, velocity;
    u8 pad94[8];
    u8 collider[0x54];
    u16 falling;
    u8 state;
    s8 slot;
    u16 unknownF4, age;
    u8 entered;
};
struct DiscPool {
    u8 pad[0x38];
    u32 allocated;
};
struct Packed {
    u32 a, b;
};
struct Map {
    u32 unknown;
    u8 *cells;
    u8 pad8[0x1c];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sprite_Remove(void *);
void Sound_PlaySong(u32);
static inline u16 Height(struct Vec *position)
{
    s32 x = position->x >> 8, z = position->z >> 8, index, height, slope;
    u8 *cell;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + (index * 4 + 12);
    slope = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline void StepDisc(struct Disc *p)
{
    Step((struct Sprite *)p, (struct Sprite *)p, &p->anim);
}
// confidence=low: disc falls under gravity, then enters its impact state at terrain height.
static inline u32 Step_UpdateFallingDisc(struct Sprite *sprite, struct Sprite *sprite2,
                                         struct Anim *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = *(const volatile u16 *const *)((u8 *)sprite + 0x2C);
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
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
            sprite2->flags |= 8;
        else
            sprite2->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */

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

inline static u16 Height_UpdateFallingDisc(struct Vec *position)
{
    u8 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 index;
    s32 height;
    s32 slope;
    u8 *cell;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + ((index * 4) + 12);
    slope = (*cell) >> 4;
    height = ((*cell) & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;

        case 2:
            height -= (u8)position->x;
            break;
    }

    return height;
}

void SunDiscPool_UpdateFallingDisc(struct DiscPool *poolArg, struct Disc *p)
{
    register struct Sprite *copy asm("r7") = (struct Sprite *)p;
    u16 height;
    struct Anim *anim = &p->anim;
    u32 t1;
    asm("" : "+r"(copy));
    if (p->entered) {
        p->flags &= ~1;
        p->age = 0;
        p->entered = 0;
    }
    asm("" : "+r"(anim));
    Step_UpdateFallingDisc((struct Sprite *)p, copy, anim);
    do {
        p->velocity.y -= 4;
    } while (0);
    {
        struct Vec *position = &p->position;
        struct Vec *velocity = &p->velocity;
        position->x = position->x + velocity->x;
        position->y = position->y + velocity->y;
        position->z = position->z + velocity->z;
    }
    *((struct Packed *)(&p->projected)) = *((struct Packed *)(&p->position));
    t1 = Height_UpdateFallingDisc(&p->position) << 16;
    height = t1 >> 16;
    p->age++;
    if (p->position.y <= 0) {
        p->flags |= 1;
        poolArg->allocated &= ~(1 << p->slot);
        Sprite_Remove(p);
        p->state = 0;
    } else if (p->position.y <= ((s16)height)) {
        p->position.y = height;
        p->velocity.x = 0;
        p->velocity.y = 0;
        p->velocity.z = 0;
        p->state = 3;
        p->entered = 1;
        *((struct Packed *)(&p->projected)) = *((struct Packed *)(&p->position));
        Sound_PlaySong(242);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SunDiscPool_UpdateFallingDisc);
#endif
