#include "saved_position_marker.h"
#include "terrain_collision.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 SavedPositionMarker_InitializeFromScript(struct SavedPositionMarker *marker, u32 unused1,
                                             u32 unused2)
{
    struct ContactSwarmSprite *secondary;
    u8 *secondaryPriority;
    struct ContactPosition *secondaryPosition;
    u8 *primaryPriority;
    u16 *paletteBase;
    struct ContactSwarmSprite *primary;
    struct ContactSwarmGraphics *secondaryGraphics;
    struct ContactSwarmGraphics *graphics;
    struct ContactPosition *position;
    u32 resourceId;
    u32 priority;
    s32 i;
    u16 *argument;
    {
        u32 variant = Script_SeekToKeyword('t');
        if (variant)
            variant = Script_GetValue();
        marker->variant = variant;
    }
    if (Script_SeekToKeyword('p')) {
        u32 cell;
        u32 height, surface;
        struct TerrainCell *properties;
        struct TerrainCellOverride *override;
        s32 x, z;
        marker->position.x = Script_GetValue();
        marker->position.y = Script_GetValue();
        marker->position.z = Script_GetValue();
        position = &marker->position;
        x = position->x >> 8;
        z = position->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            cell = 0;
        else {
            u32 row = gTerrainMap->rows[z];
            cell = row + x;
        }
        override = (struct TerrainCellOverride *)TerrainModifiers_FindCellOverride(cell, 1);
        if (override)
            properties = &override->cell;
        else
            properties = &((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
        surface = properties->shapeAndHeight >> 4;
        height = (properties->shapeAndHeight & 15) << 8;
        switch (surface) {
            case 1:
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        marker->position.y = height;
    } else {
        marker->position.x = 0;
        marker->position.y = 0;
        marker->position.z = 0;
    }
    if (Script_SeekToKeyword('R'))
        marker->scriptId = Script_GetValue();
    else
        marker->scriptId = 0;
    if (Script_SeekToKeyword('A')) {
        paletteBase = &marker->paletteBase;
        graphics = &marker->primary.graphics;
        primary = &marker->primary.sprite;
        primaryPriority = &marker->primary.sprite.priority;
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
        paletteBase = &marker->paletteBase;
        graphics = &marker->primary.graphics;
        primary = &marker->primary.sprite;
        primaryPriority = &marker->primary.sprite.priority;
        secondaryGraphics = &marker->secondary.graphics;
        secondary = &marker->secondary.sprite;
        secondaryPriority = &marker->secondary.sprite.priority;
        secondaryPosition = &marker->secondary.sprite.position;
        {
            u16 *boundary = marker->scriptArgs;
            u32 zero = 0;
            u16 *clear = boundary + 1;
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)boundary);
        }
    }
    if (marker->variant == 0) {
        u32 tile = 0x166;
        *paletteBase = tile;
    } else {
        u32 tile = 0x167;
        *paletteBase = tile;
    }
    resourceId = 0xA152;
    AnimData_Load(graphics, resourceId);
    Sprite_Init(primary, graphics, 0);
    AnimData_SelectPaletteBank(graphics, *paletteBase);
    marker->primary.sprite.flags |= 0x200;
    priority = 3;
    *primaryPriority = priority;
    marker->primary.sprite.position = marker->position;
    AnimData_Load(secondaryGraphics, resourceId);
    Sprite_Init(secondary, secondaryGraphics, 0);
    AnimData_SelectPaletteBank(secondaryGraphics, *paletteBase);
    marker->secondary.sprite.flags |= 0x400;
    *secondaryPriority = priority;
    *secondaryPosition = marker->position;
    SavedPositionMarker_SetState(marker, SavedPositionMarker_TickIdle);
    return 0;
}
