// MATCHING_CONCESSION: register-pinning
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
    register struct ContactSwarmGraphics *secondaryGraphics asm("r9");
    struct ContactSwarmGraphics *graphics;
    struct ContactPosition *position;
    u32 resourceId;
    register u32 priority asm("r4");
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
        {
            u32 offset0 = 0xB4;
            u8 *address0;
            register u8 *address1 asm("r1");
            register u8 *address2 asm("r2");
            offset0 += (u32)marker;
            paletteBase = (void *)offset0;
            graphics = &marker->primary.graphics;
            {
                register u32 offset1 asm("r1") = 0x1C;
                asm("" : "+r"(offset1));
                offset1 += (u32)marker;
                address1 = (void *)offset1;
                primary = (void *)address1;
            }
            address2 = &marker->primary.sprite.priority;
            views.primaryPriority = address2;
            offset0 = 0x98;
            offset0 += (u32)marker;
            secondaryGraphics = (void *)offset0;
            address1 = (u8 *)&marker->secondary.sprite;
            views.secondary = (void *)address1;
            address2 += 0x50;
            views.secondaryPriority = address2;
            address0 = (u8 *)&marker->secondary.sprite.position;
            views.secondaryPosition = (void *)address0;
        }
        argument = marker->scriptArgs;
        i = 1;
        do {
            *argument++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        {
            u32 offset0 = 0xB4;
            u8 *address0;
            register u8 *address1 asm("r2");
            register u8 *address2 asm("r0");
            asm("" : "+r"(offset0));
            offset0 += (u32)marker;
            paletteBase = (void *)offset0;
            graphics = &marker->primary.graphics;
            {
                register u32 offset1 asm("r2") = 0x1C;
                asm("" : "+r"(offset1));
                offset1 += (u32)marker;
                address1 = (void *)offset1;
                primary = (void *)address1;
            }
            address2 = &marker->primary.sprite.priority;
            views.primaryPriority = address2;
            offset0 = 0x98;
            asm("" : "+r"(offset0));
            offset0 += (u32)marker;
            secondaryGraphics = (void *)offset0;
            address1 = (u8 *)&marker->secondary.sprite;
            views.secondary = (void *)address1;
            address2 += 0x50;
            views.secondaryPriority = address2;
            clearBase = address2;
            address0 = (u8 *)&marker->secondary.sprite.position;
            views.secondaryPosition = (void *)address0;
        }
        {
            u16 *boundary = marker->scriptArgs;
            u16 *clear;
            u32 zero = 0;
            clear = (u16 *)(clearBase + 0x4B);
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)boundary);
        }
    }
    if (marker->variant == 0) {
        u32 tile = 0x166;
        register u16 *destination asm("r2");
        destination = paletteBase;
        asm("" : "+r"(destination));
        *destination = tile;
    } else {
        u32 tile = 0x167;
        register u16 *destination asm("r1");
        destination = paletteBase;
        asm("" : "+r"(destination));
        *destination = tile;
    }
    resourceId = 0xA152;
    AnimData_Load(graphics, resourceId);
    Sprite_Init(primary, graphics, 0);
    {
        register u16 *source asm("r2") = paletteBase;
        asm("" : "+r"(source));
        AnimData_SelectPaletteBank(graphics, *source);
    }
    marker->primary.sprite.flags |= 0x200;
    priority = 3;
    *views.primaryPriority = priority;
    marker->primary.sprite.position = marker->position;
    AnimData_Load(secondaryGraphics, resourceId);
    Sprite_Init(views.secondary, secondaryGraphics, 0);
    {
        register u16 *source asm("r2") = paletteBase;
        asm("" : "+r"(source));
        AnimData_SelectPaletteBank(secondaryGraphics, *source);
    }
    marker->secondary.sprite.flags |= 0x400;
    *views.secondaryPriority = priority;
    {
        u32 lo = *(u32 *)&marker->position;
        u32 hi = ((u32 *)&marker->position)[1];
        u32 *destination;
        asm("" : "+r"(lo), "+r"(hi));
        destination = (u32 *)views.secondaryPosition;
        destination[0] = lo;
        destination[1] = hi;
    }
    SavedPositionMarker_SetState(marker, SavedPositionMarker_TickIdle);
    return 0;
}
