#include "global.h"
#include "monster_particle_random.h"
#include "engine.h"
#include "contact_actor.h"

struct DebrisVelocity {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ChargedBurst {
    u8 unknown0[0x18];
    struct ContactPosition position;
    u8 collider[0x6C];
    u8 unknown8C[2];
    s16 height;
    u8 unknown90[0x17A];
    u16 timer;
};
void sub_0823350C(struct ChargedBurst *);
s32 Collider_Refresh(void *);
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);

static inline u32 TerrainHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell, flags, shape, height;
    u8 *data;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    flags = *data;
    shape = flags >> 4;
    {
        u32 mask = 15;
        mask &= flags;
        height = mask << 8;
    }
    switch (shape) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2:
            height -= *(u8 *)&position->x;
            break;
    }
    return height;
}

void ChargedBurst_EmitTerminalDebris(struct ChargedBurst *object)
{
    sub_0823350C(object);
    Collider_Refresh(object->collider);
    if (++object->timer > 15) {
        u32 height = object->height;
        if (height > TerrainHeight(&object->position)) {
            struct DebrisVelocity velocity, extent;
            const u16 *table = gRandomTable;
            u32 sample;
            sample = ParticleRandom_ReadDirectSample(table);
            velocity.x = (sample & 15) - 7;
            sample = ParticleRandom_ReadDirectSample(table);
            velocity.y = (sample & 31) - 16;
            sample = ParticleRandom_ReadDirectSample(table);
            velocity.z = (sample & 15) - 7;
            extent.x = 5;
            extent.y = 10;
            extent.z = 5;
            sub_08014634(4, 2, object->unknown8C, &velocity, &extent, 60, 60);
        }
        Task_Kill(object);
    }
}
