#include "global.h"
#include "scene_actor_terrain.h"
/* The original falls through without setting its result on an unobstructed path. */
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
void SpriteEffectController_Create(struct ContactPosition *, struct ContactPosition *, s32, u32,
                                   u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32,
                                   s32);
static inline u8 *FeatherEffectPool_CellDataAndCenter(s32 cell, struct ContactPosition *center,
                                                      struct ContactPosition **output)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    *output = center;
    if (data)
        return data + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline void FeatherEffectPool_GetCellCenter(s32 cell, struct ContactPosition *center,
                                                   struct ContactPosition **result)
{
    u8 *data;
    s32 x = Mod(cell, SceneActorTerrain_Width());
    s32 z = Div(cell, SceneActorTerrain_Width());
    center->x = (x << 8) + 128;
    data = FeatherEffectPool_CellDataAndCenter(cell, center, result);
    center->y = (*data & 15) << 8;
    center->z = (z << 8) + 128;
}
static inline void FeatherEffectPool_ClearWords(const void *source, void *destination, u32 control)
{
    CpuSet(source, destination, control);
}
#ifdef NONMATCHING
// fuzzy: 98.1%
// Tentative feather identity; clips the scatter position at rising terrain.
u8 FeatherEffectPool_ClipScatterPath(struct Feather *p, struct ContactPosition *spot, s32 current,
                                     s32 target, s32 direction)
{
    struct {
        u32 zero;
        struct ContactPosition velocity;
        struct ContactPosition center;
    } local;
    s32 previous;
    s32 height;
    struct ContactPosition *velocity = &local.velocity;
    local.zero = 0;
    FeatherEffectPool_ClearWords(&local.zero, velocity, 0x05000002);
    if (current != target) {
        do {
            previous = current;
            switch (direction) {
                case 0:
                    current++;
                    break;

                case 1:
                    current += SceneActorTerrain_Width();
                    break;

                case 2:
                    current--;
                    break;

                case 3:
                    current -= SceneActorTerrain_Width();
                    break;
            }

            if ((*SceneActorTerrain_GetCell(current)) >> 4)
                return 1;
            height = (*SceneActorTerrain_GetCell(previous)) & 15;
            if (height < ((*SceneActorTerrain_GetCell(current)) & 15)) {
                struct ContactPosition *center;
                FeatherEffectPool_GetCellCenter(current, &local.center, &center);
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
                    const u16 *table = gRandomTable;
                    u32 *rng = &gRandomIndex;
                    u32 index;
                    u32 zero;
                    index = ((*rng) + 1) & 1023;
                    *rng = index;
                    {
                        u16 v1 = table[index];
                        spot->y = (height << 8) + (v1 & 255);
                    }
                    zero = 0;
                    if (!p->variant) {
                        s32 life;
                        u16 nextIndex = (index + 1) & 1023;
                        *rng = nextIndex;
                        {
                            u16 v2 = table[nextIndex];
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
