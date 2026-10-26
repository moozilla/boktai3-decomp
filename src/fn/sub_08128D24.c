// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 88.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: terrain/center allocation and pointer/register lifetimes.
// The original falls through on the unobstructed path without setting its result.
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
static inline u32 Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
static inline s32 CellIndex(struct Vec *p)
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
void SpriteEffectController_Create(struct Vec *, struct Vec *, s32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32, s32, s32, s32, s32, s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
// confidence=low: scans intervening cells and clips the scatter position at rising terrain.
u8 FeatherEffectPool_ClipScatterPath(struct Feather *p, struct Vec *spot, s32 current, s32 target,
                                     s32 inputDirection)
{
    struct {
        u32 zero;
        struct Vec velocity;
        struct Vec center;
    } local;
    register s32 direction asm("r9") = inputDirection;
    s32 previous;
    s32 height;
    struct Vec *velocity = &local.velocity;
    local.zero = 0;
    CpuSet(&local.zero, velocity, 0x05000002);
    if (current != target) {
        do {
            previous = current;
            switch (direction) {
                case 0:
                    current++;
                    break;

                case 1:
                    current += *((s16 *)(gTerrainMap->cells + 4));
                    break;

                case 2:
                    current--;
                    break;

                case 3:
                    current -= *((s16 *)(gTerrainMap->cells + 4));
                    break;
            }

            if ((*CellData(current)) >> 4)
                return 1;
            height = (*CellData(previous)) & 15;
            if (height < ((*CellData(current)) & 15)) {
                register struct Vec *center asm("r6") = &local.center;
                s32 x;
                register s32 z asm("r8");
                asm("" : "+r"(center));
                x = Mod(current, *((s16 *)(gTerrainMap->cells + 4)));
                z = Div(current, *((s16 *)(gTerrainMap->cells + 4)));
                center->x = (x << 8) + 128;
                center->y = ((*CellData(current)) & 15) << 8;
                center->z = (z << 8) + 128;
                switch (direction) {
                    case 0:
                        spot->x = center->x - 128;
                        return 1;

                    case 1:
                        spot->z = center->z - 128;
                        return 1;

                    case 2:
                        spot->x = center->x + 128;
                        break;

                    case 3:
                        spot->z = center->z + 128;
                        break;
                }

                {
                    register const u16 *table asm("r5") = (const u16 *)0x0203B400;
                    register u32 *rng asm("r4") = &gRandomIndex;
                    register u32 mask asm("r6") = 1023;
                    u32 index;
                    register u32 zero asm("r3");
                    asm("" : "+r"(table), "+r"(rng), "+r"(mask));
                    {
                        u8 k1 = 1;
                        index = ((*rng) + k1) & mask;
                    }
                    *rng = index;
                    {
                        typeof(table[index]) v1 = table[index];
                        spot->y = (height << 8) + (v1 & 255);
                    }
                    zero = 0;
                    asm("" : "+r"(zero));
                    if (!p->variant) {
                        s32 life;
                        index = ((*rng) + 1) & mask;
                        *rng = index;
                        {
                            typeof(table[index]) v2 = table[index];
                            life = (v2 & 63) + 30;
                        }
                        SpriteEffectController_Create(spot, &local.velocity, 0x1C1A, 0xD1B8, 3,
                                                      zero, 0x31, zero, 1, zero, zero, life, 4, 1,
                                                      p->effectFlags, 30, 8, 8);
                    } else {
                        SpriteEffectController_Create(spot, &local.velocity, 0x1C1A, 0xD1B8, 0x12,
                                                      zero, 0x11, zero, 1, zero, zero, zero, 4, 1,
                                                      p->effectFlags, 30, 8, 8);
                    }
                }
                return 1;
            }
        } while (current != target);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", FeatherEffectPool_ClipScatterPath);
#endif
