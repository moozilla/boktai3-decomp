#include "effects/feathers/feather_effect_pool.h"
#include "engine/math/monster_particle_random.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/terrain/terrain_sampling.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "bosses/vanargand/vanargand_parts.h"
#include "monsters/ghoul_disc.h"

void SpriteEffectController_Create(struct ContactPosition *, struct ContactPosition *, s32, u32,
                                   u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32,
                                   s32);

// confidence=low: scatters terrain-constrained sprite effects ahead of the feather animation.
void FeatherEffectPool_SpawnGroundScatter(struct Feather *feather, s32 count)
{
    struct ContactPosition center, velocity, spot;
    s32 direction = Ghoul_QuarterDirection((feather->angle + 0x20) & 255) * 64;
    s32 sourceCell, i;

    MakeOffset(&center, direction, 512);
    Vanargand_AddPosition(&center, &feather->sprite.position, &center);
    sourceCell = Terrain_GetCellIndex(&feather->sprite.position);
    ZeroFill8(&velocity);
    for (i = 0; i < count; i++) {
        s32 cell, height, floor;
        /* First the random table, then the terrain cell being tested. */
        const u8 *data = (const u8 *)gRandomTable;

        spot.x = (ParticleRandom_NextSample((const u16 *)data, &gRandomIndex, 1023) & 255) - 128;
        spot.y = 0;
        spot.z = (ParticleRandom_NextSample((const u16 *)data, &gRandomIndex, 1023) & 255) - 128;
        Vanargand_AddPosition(&spot, &center, &spot);
        cell = Terrain_GetCellIndex(&spot);
        if (FeatherEffectPool_ClipScatterPath(feather, &spot, sourceCell, cell,
                                              Ghoul_QuarterDirection(direction)))
            continue;
        if (*(data = SceneActorTerrain_GetCell(cell)) >> 4)
            continue;
        height = *SceneActorTerrain_GetCell(sourceCell) & 15;
        {
            s32 value = *SceneActorTerrain_GetCell(cell);
            s32 mask = 15;

            mask &= value;
            floor = mask;
        }
        if (abs(height - floor) > 1)
            continue;
        spot.y = floor << 8;
        if (feather->variant == 0) {
            s32 life = (ParticleRandom_ReadSample((const u16 *)RANDOM_TABLE_ADDR) & 63) + 20;

            SpriteEffectController_Create(&spot, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 1, 0, 0,
                                          life, 4, 1, feather->effectFlags, 30, 8, 8);
        } else {
            SpriteEffectController_Create(&spot, &velocity, 0x1C1A, 0xD1B8, 0x12, 0, 0x11, 0, 1, 0,
                                          0, 0, 4, 1, feather->effectFlags, 30, 8, 8);
        }
    }
}
