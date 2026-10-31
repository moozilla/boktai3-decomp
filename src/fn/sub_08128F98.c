#include "global.h"
#include "engine.h"
#include "scene_actor_registry.h"
// Packed-position address lowering and constructor constant lifetimes still differ.
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
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
static inline u32 Random(const u16 *table, u32 *rng)
{
    u32 index = (*rng + 1) & 1023;
    *rng = index;
    return *(const volatile u16 *)(index * 2 + (u32)table);
}
static inline s32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
u8 *TerrainModifiers_FindCellOverride(u32, u32);
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        p += 4;
    else {
        struct EngineMap *map = gTerrainMap;
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
void SpriteEffectController_Create(struct ContactPosition *, struct ContactPosition *, s32, u32,
                                   u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32,
                                   s32);
u8 FeatherEffectPool_ClipScatterPath(struct Feather *, struct ContactPosition *, s32, s32, s32);
// confidence=low: scatters terrain-constrained sprite effects ahead of the feather animation.
static inline void SetVelocity(struct ContactPosition *v, s32 angle, s32 speed)
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
static inline void Add(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
#ifdef NONMATCHING
// fuzzy: 82.7%
// scope shape 10 (differences once low registers are set aside)
void FeatherEffectPool_SpawnGroundScatter(struct Feather *p, s32 count)
{
    struct ContactPosition center;
    struct ContactPosition velocity;
    union SceneActorRegistryPackedPosition spot;
    s32 sourceCell;
    s32 direction = Divide64((p->angle + 0x20) & 255) * 64;
    s32 cell;
    SetVelocity(&center, direction, 512);
    Add(&center, &p->position, &center);
    {
        s32 current = CellIndex(&p->position);
        sourceCell = current;
    }
    ZeroFill8(&velocity);
    if (count > 0) {
        u32 *rng = &gRandomIndex;
        s32 remaining = count;
        do {
            const void *data = gRandomTable;
            s32 height;
            s32 difference;
            spot.packed.x = (Random((const u16 *)data, rng) & 255) - 128;
            spot.packed.y = 0;
            spot.packed.z = (Random((const u16 *)data, rng) & 255) - 128;
            Add(&spot.position, &center, &spot.position);
            cell = CellIndex(&spot.position);
            if (FeatherEffectPool_ClipScatterPath(p, &spot.position, sourceCell, cell,
                                                  Divide64(direction)))
                goto next;
            data = CellData(cell);
            if ((*(const u8 *)data) >> 4)
                goto next;
            height = (*CellData(sourceCell)) & 15;
            {
                s32 value = *CellData(cell);
                s32 mask = 15;
                mask &= value;
                difference = mask;
            }
            {
                s32 delta = height - difference;
                if (delta < 0)
                    delta = -delta;
                if (delta > 1)
                    goto next;
            }
            {
                u32 zero;
                u8 one;
                spot.position.y = difference << 8;
                zero = 0;
                one = 1;
                if (!p->variant) {
                    s32 life = (((u8)Random((const u16 *)RANDOM_TABLE_ADDR, rng)) & 63) + 20;
                    SpriteEffectController_Create(&spot.position, &velocity, 0x1C1A, 0xD1B8, 3,
                                                  zero, 0x31, zero, one, zero, zero, life, 4, one,
                                                  p->effectFlags, 30, 8, 8);
                } else {
                    SpriteEffectController_Create(&spot.position, &velocity, 0x1C1A, 0xD1B8, 0x12,
                                                  zero, 0x11, zero, one, zero, zero, zero, 4, one,
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
