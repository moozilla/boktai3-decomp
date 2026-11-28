#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"
#include "scene/motion/scene_actor_terrain.h"

void TrapSpellParticles_CreateDefaultBurst(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                           u32);

static inline s32 groundHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    struct ContactPosition *savedPosition = position;
    s32 index, height;
    u8 *cell;
    u32 kind;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)savedPosition->z;
            break;
        case 2:
            height -= (u8)savedPosition->x;
            break;
    }
    return height;
}
static inline void ClearMotion(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
void LootDeposit_UpdateMemberFall(void *unused, struct CallbackPoolEntry *entry)
{
    s32 i;
    if (entry->defeated) {
        entry->ground = groundHeight(&entry->sprite.position);
        entry->defeated = 0;
        ClearMotion(&entry->motion);
    } else {
        entry->sprite.position.y -= entry->motion.y;
        entry->motion.y += 2;
        if (entry->ground >= entry->sprite.position.y) {
            entry->sprite.position.y = entry->ground;
            entry->callbackIndex = 4;
            entry->defeated = 1;
            for (i = 0; i < 8; i++)
                TrapSpellParticles_CreateDefaultBurst(&entry->sprite.position, 8, 16, 4, 12, 18,
                                                      i << 5, 8, 64, 1, 1, 0);
        }
    }
}
