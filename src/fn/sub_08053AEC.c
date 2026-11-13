// MATCHING_CONCESSION: register-pinning
#include "interaction_marker.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneBody_Init(void *, u32, void *, u32, u32, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 InteractionMarker_InitializeFromScript(struct InteractionMarker *marker, u32 id, u32 unused)
{
    struct ContactPosition *primaryPosition;
    struct ContactPosition *position;
    u8 *secondaryPriority;
    struct ContactPosition *secondaryPosition;
    struct ContactSwarmSprite *primary;
    struct ContactSwarmGraphics *secondaryGraphics;
    u8 *primaryPriority;
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *secondary;
    u32 frame;
    u32 priority;
    u16 *paletteBase;
    u32 resourceId;
    u32 keyword;
    s32 i;
    u16 *argument;
    marker->id = id;
    keyword = Script_SeekToKeyword('p');
    if (keyword) {
        u32 cell;
        u32 height, surface;
        u8 *properties;
        register struct ContactPosition *point asm("r4");
        register s16 *yAddress asm("r5");
        s32 x, z;
        {
            u32 value = Script_GetValue();
            point = &marker->position;
            point->x = value;
        }
        {
            u32 value = Script_GetValue();
            yAddress = &marker->position.y;
            *yAddress = value;
        }
        marker->position.z = Script_GetValue();
        x = point->x >> 8;
        z = point->z >> 8;
        position = point;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            cell = 0;
        else {
            u32 row = gTerrainMap->rows[z];
            cell = row + x;
        }
        properties = TerrainModifiers_FindCellOverride(cell, 1);
        if (properties)
            properties += 4;
        else {
            struct EngineMap *map = gTerrainMap;
            u32 offset = cell * 4 + 12;
            properties = map->cells + offset;
        }
        surface = *properties >> 4;
        height = (*properties & 15) << 8;
        switch (surface) {
            case 1:
                height -= (u8)point->z;
                break;
            case 2:
                height -= (u8)point->x;
                break;
        }
        *yAddress = height;
    } else {
        marker->position.x = keyword;
        marker->position.y = keyword;
        marker->position.z = keyword;
        position = &marker->position;
    }
    if (Script_SeekToKeyword('s'))
        marker->scriptId = Script_GetValue();
    else
        marker->scriptId = 0;
    if (Script_SeekToKeyword('A')) {
        graphics = &marker->primary.graphics;
        primary = &marker->primary.sprite;
        primaryPriority = &marker->primary.sprite.priority;
        primaryPosition = &marker->primary.sprite.position;
        secondaryGraphics = &marker->secondary.graphics;
        secondary = &marker->secondary.sprite;
        secondaryPriority = &marker->secondary.sprite.priority;
        secondaryPosition = &marker->secondary.sprite.position;
        argument = marker->scriptArgs;
        i = 1;
        do {
            *argument++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        graphics = &marker->primary.graphics;
        primary = &marker->primary.sprite;
        primaryPriority = &marker->primary.sprite.priority;
        primaryPosition = &marker->primary.sprite.position;
        secondaryGraphics = &marker->secondary.graphics;
        secondary = &marker->secondary.sprite;
        secondaryPriority = &marker->secondary.sprite.priority;
        secondaryPosition = &marker->secondary.sprite.position;

        {
            u32 zero = 0;
            u16 *clear;
            i = 1;
            clear = &marker->scriptArgs[1];
            do {
                *clear-- = zero;
            } while (--i >= 0);
        }
    }
    {
        u32 enabled;
        enabled = Script_SeekToKeyword('h');
        if (enabled)
            marker->bodyEnabled = Script_GetValue();
        else
            marker->bodyEnabled = enabled;
    }
    if (marker->bodyEnabled) {
        SceneBody_Init(marker->body, marker->id, position, 0, 7, marker);
        TerrainOverlay_QueueAddActor(marker->body);
    }
    paletteBase = &marker->paletteBase;
    *paletteBase = 0xA9;
    {
        u32 value = 0xBA8;
        resourceId = value;
    }
    AnimData_Load(graphics, resourceId);
    Sprite_Init(primary, graphics, 0);
    frame = 7;
    {
        struct ContactSwarmSprite *write = primary;
        write->frame = frame;
    }
    AnimData_SelectPaletteBank(graphics, *paletteBase);
    marker->primary.sprite.flags |= 0x200;
    priority = 3;
    {
        u8 *write = primaryPriority;
        *write = priority;
    }
    *primaryPosition = *position;
    AnimData_Load(secondaryGraphics, resourceId);
    Sprite_Init(secondary, secondaryGraphics, 0);
    secondary->frame = frame;
    {
        u32 tile = *paletteBase;
        AnimData_SelectPaletteBank(secondaryGraphics, tile);
    }
    {
        struct ContactSwarmSprite *read = secondary;
        u32 flags;
        flags = read->flags | 0x400;
        {
            struct ContactSwarmSprite *write = secondary;
            write->flags = flags;
        }
    }
    {
        u8 *write = secondaryPriority;
        *write = priority;
    }
    *secondaryPosition = *position;
    InteractionMarker_SetState(marker, InteractionMarker_TickIdle);
    return 0;
}