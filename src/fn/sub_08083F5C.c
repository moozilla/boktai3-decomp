// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
/* SceneActorControl_ReflectTerrainProbe: Reverses secondary horizontal motion and stops primary horizontal motion when projected terrain exceeds current ground height. */
#define SceneActorControl_ReflectTerrainProbe sub_08083F5C
#define SceneActorControl_ReflectTerrainProbe sub_08083F5C
void SceneActorControl_ReflectTerrainProbe(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition next;
    struct ContactPosition *sum = &next;
    const struct ContactPosition *position;
    const struct ContactPosition *motion;
    s32 x, z;
    u32 cell;
    s32 height;
    struct ContactPosition *terrainPosition;
    u8 *data;
    position = &actor->position;
    motion = &actor->secondaryMotion;
    {
        u32 first = (u16)position->x, second = (u16)motion->x;
        next.x = first + second;
    }
    {
        u32 first = (u16)position->y;
        u32 second;
        second = (u16)motion->y;
        sum->y = first + second;
    }
    {
        u32 first = (u16)position->z;
        register u32 second asm("r3");
        u32 packed;
        u32 word;
        second = (u16)motion->z;
        word = first + second;
        sum->z = word;
        packed = (u16)next.x;
        packed <<= 16;
        x = (s32)packed >> 24;
        z = (s32)(word << 16) >> 24;
    }
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    terrainPosition = &next;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = (u8 *)((u32)map->cells + offset);
    }
    {
        register u32 flags asm("r1") = *data;
        u32 shape = flags >> 4;
        u32 adjustment;
        height = (flags & 15) << 8;
        switch (shape) {
            case 1:
                adjustment = *(u8 *)&terrainPosition->z;
                break;
            case 2:
                adjustment = *(u8 *)&terrainPosition->x;
                break;
            default:
                goto compare_ground;
        }
        height -= (s32)adjustment;
    }
compare_ground: {
    u32 offset = 0x2BE;
    u16 *groundField;
    u32 ground;
    groundField = (void *)((u8 *)actor + offset);
    ground = *groundField;
    if ((s32)ground < height) {
        u32 offset = 0x2EC;
        register s16 *component asm("r1");
        register s32 value asm("r0");
        register u32 zero asm("r2");
        component = (void *)((u8 *)actor + offset);
        value = *component;
        value = -value;
        zero = 0;
        *component = value;
        offset += 4;
        component = (void *)((u8 *)actor + offset);
        value = *component;
        *component = -value;
        {
            s16 *output = &actor->motion.x;
            *output = zero;
            output += 2;
            *output = zero;
        }
    }
}
}
