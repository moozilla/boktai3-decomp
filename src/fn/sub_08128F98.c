// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Vector address lifetimes, RNG allocation and packed coordinate stores still differ.
struct Vec {
    s16 x, y, z, pad;
};
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    u8 animation[0x10];
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};
static inline u32 Random(u32 *rng)
{
    u32 index = (*rng + 1) & 1023;
    *rng = index;
    return *(const volatile u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
static inline s32 CellIndex(struct Vec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
u8 *TerrainModifiers_FindCellOverride(u32, u32);
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        p += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = index * 4 + 12;
        p = map->cells + offset;
    }
    return p;
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
void SpriteEffectController_Create(struct Vec *, struct Vec *, s32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32, s32, s32, s32, s32, s32, s32);
u8 FeatherEffectPool_ClipScatterPath(struct Feather *, struct Vec *, s32, s32, s32);
// confidence=low: scatters terrain-constrained sprite effects ahead of the feather animation.
static inline void SetVelocity(struct Vec *v, s32 angle, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline s32 Divide64(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -(-n >> 6);
}
static inline void Add(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void FeatherEffectPool_SpawnGroundScatter(struct Feather *p, s32 count)
{
    struct {
        u32 zero;
        struct Vec center;
        struct Vec velocity;
        struct Vec spot;
        s32 cell;
    } local;
    s32 direction = Divide64((p->angle + 0x20) & 255) * 64;
    s32 cell;
    s32 t1;
    SetVelocity(&local.center, direction, 512);
    Add(&local.center, &p->position, &local.center);
    {
        s32 sourceCell = CellIndex(&p->position);
        local.cell = sourceCell;
    }
    local.zero = 0;
    t1 = 0x05000002;
    CpuSet(&local.zero, &local.velocity, t1);
    if (count > 0) {
        u32 *rng = &gUnk_03005308;
        s32 remaining = count;
        do {
            s32 height;
            s32 difference;
            *((u32 *)(&local.spot)) = (u16)((Random(rng) & 255) - 128);
            *((u32 *)(&local.spot.z)) = (u16)((Random(rng) & 255) - 128);
            Add(&local.spot, &local.center, &local.spot);
            cell = CellIndex(&local.spot);
            if (FeatherEffectPool_ClipScatterPath(p, &local.spot, local.cell, cell,
                                                  Divide64(direction)))
                goto next;
            if ((*CellData(cell)) >> 4)
                goto next;
            height = (*CellData(local.cell)) & 15;
            {
                s32 value = *CellData(cell);
                difference = value & 15;
            }
            {
                s32 delta = height - difference;
                if (delta < 0)
                    delta = -delta;
                if (delta > 1)
                    goto next;
            }
            {
                u32 zero = 0;
                local.spot.y = difference << 8;
                if (!p->variant) {
                    s32 life = (((u8)Random(rng)) & 63) + 20;
                    SpriteEffectController_Create(&local.spot, &local.velocity, 0x1C1A, 0xD1B8, 3,
                                                  zero, 0x31, zero, 1, zero, zero, life, 4, 1,
                                                  p->effectFlags, 30, 8, 8);
                } else {
                    SpriteEffectController_Create(&local.spot, &local.velocity, 0x1C1A, 0xD1B8,
                                                  0x12, zero, 0x11, zero, 1, zero, zero, zero, 4, 1,
                                                  p->effectFlags, 30, 8, 8);
                }
            }
        next:
            remaining--;

        } while (remaining);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", FeatherEffectPool_SpawnGroundScatter);
#endif
