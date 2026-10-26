// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "contact_actor.h"

struct Unk08233760PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Unk08233760Object {
    u8 unknown0[0x18];
    struct ContactPosition position;
    u8 collider[0x6C];
    u8 unknown8C[2];
    s16 height;
    u8 unknown90[0x17A];
    u16 timer;
};
void sub_0823350C(struct Unk08233760Object *);
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

void ChargedBurst_EmitTerminalDebris(struct Unk08233760Object *object)
{
    sub_0823350C(object);
    Collider_Refresh((u8 *)object + 0x20);
    if (++object->timer > 15) {
        u32 height = object->height;
        if (height > TerrainHeight(&object->position)) {
            struct Unk08233760PackedVec velocity, extent;
            const u16 *table = gUnk_0203B400;
            u32 *seed = &gRandomIndex;
            register u32 i asm("r2") = *seed + 1;
            u32 mask = 1023;
            register u32 low asm("r9");
            i &= mask;
            {
                u32 sample = table[i];
                low = 15;
                asm("" : "+r"(low));
                velocity.x = (sample & low) - 7;
            }
            i = (i + 1) & mask;
            {
                u32 sample = table[i];
                u32 mask31 = 31;
                velocity.y = (sample & mask31) - 16;
            }
            i = (i + 1) & mask;
            *seed = i;
            velocity.z = (table[i] & low) - 7;
            extent.x = 5;
            extent.y = 10;
            extent.z = 5;
            sub_08014634(4, 2, (u8 *)object + 0x8C, &velocity, &extent, 60, 60);
        }
        Task_Kill(object);
    }
}
