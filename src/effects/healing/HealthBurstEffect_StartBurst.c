#include "global.h"
#include "engine/engine.h"
#include "engine/video/render.h"
#include "engine/math/math_shift.h"
#include "engine/math/monster_particle_random.h"
#include "scene/terrain/terrain_sampling.h"
#include "effects/healing/healing_effect.h"

/* Drops the next burst sprite from above the camera, at a random point on one quarter of a ring,
   and records the terrain height it will land on. */
void HealthBurstEffect_StartBurst(struct HealthBurstEffect *effect)
{
    struct HealthBurstSprite *sprite = &effect->sprites[effect->nextSprite];
    s32 sector, angle, radius, x, z, cell;
    u8 *properties;
    s32 flags;
    s32 ringX;

    sector = Div(effect->timer, 18);
    angle = (sector * 64 + (ParticleRandom_ReadDirectSample(gRandomTable) & 63)) & 255;
    radius = (ParticleRandom_ReadDirectSample(gRandomTable) & 255) + 128;
    ringX = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * radius, 12);
    x = ringX;
    z = Math_ShiftTowardZero(radius * gUnk_086149C4[angle] * 3, 13);
    sprite->flags &= ~1;
    sprite->position = *(struct ContactPosition *)&gCamera.position;
    sprite->position.x += x - z - 75;
    sprite->position.y += 850;
    sprite->position.z += x + z - 75;
    effect->spriteState[effect->nextSprite] = 1;
    effect->spriteTimer[effect->nextSprite] = 0;
    cell = Terrain_GetCellIndex(&sprite->position);
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties = (u8 *)&((struct TerrainCellOverride *)properties)->cell;
    else
        properties = (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
    flags = ((struct TerrainCell *)properties)->flags;
    if (flags & 0x40)
        effect->floorHeight[effect->nextSprite] = 0;
    else
        effect->floorHeight[effect->nextSprite] = Terrain_SampleHeight(&sprite->position);
    effect->nextSprite = 1 - effect->nextSprite;
}
