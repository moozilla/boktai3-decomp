// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
extern struct Map *gTerrainMap;
extern u32 gTerrainWidth, gTerrainHeight;
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sprite_Remove(void *);
void Sound_PlaySong(u32);

s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

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

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void Polar(struct Vec *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

static inline u8 TestFalling(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}

static inline void Add(struct Vec *out, struct Vec *a, struct Vec *b)
{
    u32 value = (u16)a->x;
    out->x = value + (u16)b->x;
    value = (u16)a->y;
    out->y = value + (u16)b->y;
    value = (u16)a->z;
    out->z = value + (u16)b->z;
}
static inline void Subtract(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

#ifdef NONMATCHING
// fuzzy: 87.0%
// scope shape 20 (differences once low registers are set aside)
// confidence=low: updates a disc's sprite and position each frame, advancing its animation and falling state
void sub_08129C64(struct DiscPool *pool, struct Disc *p)
{
    struct Sprite *sprite = (struct Sprite *)p;
    struct Anim *anim = &p->anim;
    struct Vec ahead, delta;
    register s16 *velocityZ asm("r6");
    u16 height;
    if (p->entered) {
        p->flags &= ~1;
        p->age = 0;
        p->entered = 0;
    }
    {

        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 ended, duration, flags, bit;
        {
            u32 offset = anim->index << 1;
            const volatile u16 *table;
            table = *(const volatile u16 *const *)((u8 *)sprite + 0x2C);
            command = (const volatile u16 *)((u32)table + offset);
        }
        p->frame = *command >> 6;
        {
            u32 input = anim->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));

            first = bit;
            first &= input;
            asm("" : "+r"(first));
            if (first != (((*command & 0x30) >> 4) & bit))
                p->flags |= 4;
            else
                p->flags &= ~4;
        }
        {
            u32 input = anim->flags;
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
            u32 elapsed = anim->elapsed + 1;
            register u32 mask asm("r5");
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            anim->elapsed = elapsed;
            temp = 0xFFFF;

            asm("" : "+r"(temp));
            mask = temp;

            duration = anim->duration;
            elapsed = (u16)elapsed;
            savedDuration = duration;
            if (elapsed >= duration) {
                anim->elapsed = 0;
                flags = anim->flags;
                if (flags & 4) {
                    u32 index = anim->index;
                    if (index == 0) {
                        u32 hold = 8;
                        hold &= flags;
                        if (hold) {
                            anim->elapsed = savedDuration;
                        } else {
                            anim->index = anim->frameCount - 1;
                        }
                        ended = 1;
                    } else {
                        anim->index = index - 1;
                        ended = 0;
                    }
                } else {
                    u32 index = anim->index + 1;
                    u32 frameCount;
                    u32 endFlag;
                    anim->index = index;
                    frameCount = anim->frameCount;
                    if ((index & mask) >= frameCount) {
                        endFlag = 8;
                        endFlag &= flags;

                        endFlag = (u8)endFlag;
                        if (endFlag) {
                            anim->elapsed = duration;
                            {
                                u32 lastIndex = frameCount - 1;
                                anim->index = lastIndex;
                            }
                        } else {
                            anim->index = 0;
                        }
                        ended = 1;
                    } else {
                        ended = 0;
                    }
                }
                {
                    u32 offset = anim->index << 1;
                    const volatile u16 *table;
                    table = anim->commands;
                    command = (const volatile u16 *)((u32)table + offset);
                }
                anim->frameWeight = *command & 15;
                {
                    register s32 weight asm("r1") = anim->frameWeight;
                    s32 speed;
                    speed = anim->speed;
                    speed = (speed * weight) >> 6;
                    anim->duration = speed;
                    if (!(u8)speed)
                        anim->duration = 1;
                }
            } else {
                ended = 0;
            }
        }
    }
    Add(&sprite->position, &sprite->position, &p->velocity);
    *(struct Packed *)&p->projected = *(struct Packed *)&p->position;
    Polar(&ahead, Math_Atan2(p->velocity.x, p->velocity.z), 32);
    velocityZ = &p->velocity.z;
    Add(&ahead, &ahead, &sprite->position);
    if (!TestFalling(&p->falling, 1)) {
        struct Vec *previous = &p->projected;
        Subtract(&delta, &p->target, previous);
        if (delta.x * delta.x + delta.z * delta.z <= 0xBD0F) {
            p->falling |= 1;
            p->velocity.y -= 4;
        }
    } else {
        p->velocity.y -= 4;
    }
    {
        u32 result = Height(&ahead) << 16;
        height = result >> 16;
    }
    Collider_SetPosition(p->collider, &sprite->position, 0);
    Collider_Refresh(p->collider);
    p->age++;
    if (sprite->position.y <= 0) {
        sprite->flags |= 1;
        pool->allocated &= ~(1 << p->slot);
        Sprite_Remove(p);
        p->state = 0;
    } else if (sprite->position.y <= (s16)height) {
        sprite->position.x = sprite->position.x - p->velocity.x;
        sprite->position.y = sprite->position.y - p->velocity.y;
        sprite->position.z = sprite->position.z - *velocityZ;
        p->velocity.x = 0;
        p->velocity.y = 0;
        *velocityZ = 0;
        p->state = 3;
        p->entered = 1;
        *(struct Packed *)&p->projected = *(struct Packed *)&p->position;
        Sound_PlaySong(242);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08129C64);
#endif
