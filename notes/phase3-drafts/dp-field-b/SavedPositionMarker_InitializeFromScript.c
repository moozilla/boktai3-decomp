#include "saved_position_marker.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 SavedPositionMarker_InitializeFromScript(struct SavedPositionMarker *marker, u32 unused1,
                                             u32 unused2)
{
    struct {
        struct ContactSwarmSprite *secondary;
        u8 *secondaryPriority;
        struct ContactPosition *secondaryPosition;
        u8 *primaryPriority;
    } views;
    u16 *paletteBase;
    struct ContactSwarmSprite *primary;
    struct ContactSwarmGraphics *secondaryGraphics;
    struct ContactSwarmGraphics *graphics;
    struct ContactPosition *position;
    u32 resourceId;
    u32 priority;
    s32 i;
    u16 *argument;
    u8 *clearBase;
    {
        u32 variant = Script_SeekToKeyword('t');
        if (variant)
            variant = Script_GetValue();
        marker->variant = variant;
    }
    if (Script_SeekToKeyword('p')) {
        u32 cell;
        u32 height, surface;
        u8 *properties;
        struct ContactPosition *point;
        s16 *yAddress;
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
        position = point;
        x = position->x >> 8;
        z = position->z >> 8;
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
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        *yAddress = height;
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
        views.primaryPriority = &marker->primary.sprite.priority;
        secondaryGraphics = &marker->secondary.graphics;
        views.secondary = &marker->secondary.sprite;
        views.secondaryPriority = &marker->secondary.sprite.priority;
        views.secondaryPosition = &marker->secondary.sprite.position;
        argument = marker->scriptArgs;
        i = 1;
        do {
            *argument++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        paletteBase = &marker->paletteBase;
        graphics = &marker->primary.graphics;
        primary = &marker->primary.sprite;
        views.primaryPriority = &marker->primary.sprite.priority;
        secondaryGraphics = &marker->secondary.graphics;
        views.secondary = &marker->secondary.sprite;
        views.secondaryPriority = &marker->secondary.sprite.priority;
        views.secondaryPosition = &marker->secondary.sprite.position;
        {
            u16 *boundary = marker->scriptArgs;
            u16 *clear;
            u32 zero = 0;
            clear = &marker->scriptArgs[1];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)boundary);
        }
    }
    if (marker->variant == 0) {
        u32 tile = 0x166;
        u16 *destination;
        destination = paletteBase;
        *destination = tile;
    } else {
        u32 tile = 0x167;
        u16 *destination;
        destination = paletteBase;
        *destination = tile;
    }
    resourceId = 0xA152;
    AnimData_Load(graphics, resourceId);
    Sprite_Init(primary, graphics, 0);
    {
        u16 *source = paletteBase;
        AnimData_SelectPaletteBank(graphics, *source);
    }
    marker->primary.sprite.flags |= 0x200;
    priority = 3;
    *views.primaryPriority = priority;
    marker->primary.sprite.position = marker->position;
    AnimData_Load(secondaryGraphics, resourceId);
    Sprite_Init(views.secondary, secondaryGraphics, 0);
    {
        u16 *source = paletteBase;
        AnimData_SelectPaletteBank(secondaryGraphics, *source);
    }
    marker->secondary.sprite.flags |= 0x400;
    *views.secondaryPriority = priority;
    {
        struct ContactPosition saved = marker->position;
        *views.secondaryPosition = saved;
    }
    SavedPositionMarker_SetState(marker, SavedPositionMarker_TickIdle);
    return 0;
}
