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
    register struct ContactSwarmSprite *secondary asm("r10");
    u32 frame;
    register u32 priority asm("r5");
    u16 *paletteBase;
    register u32 resourceId asm("r8");
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
        {
            u32 graphicsOffset = 0x94;
            u8 *address0;
            register u8 *address1 asm("r1");
            register u8 *address2 asm("r2");
            asm("" : "+r"(graphicsOffset));
            graphicsOffset += (u32)marker;
            address0 = (void *)graphicsOffset;
            graphics = (void *)address0;
            address1 = (u8 *)&marker->primary.sprite;
            primary = (void *)address1;
            address2 = &marker->primary.sprite.priority;
            asm("" : "+r"(address2));
            primaryPriority = address2;
            address0 = (u8 *)&marker->primary.sprite.position;
            primaryPosition = (void *)address0;
            address1 += 0x7C;
            secondaryGraphics = (void *)address1;
            address2 = (u8 *)&marker->secondary.sprite;
            secondary = (void *)address2;
            address0 += 0x3B;
            secondaryPriority = address0;
            address1 -= 0x10;
            secondaryPosition = (void *)address1;
        }
        {
            u32 offset = 0x108;
            argument = (u16 *)((u8 *)marker + offset);
        }
        i = 1;
        do {
            *argument++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        {
            u32 graphicsOffset = 0x94;
            u8 *address0;
            register u8 *address1 asm("r1");
            u8 *address2;
            graphicsOffset += (u32)marker;
            address0 = (void *)graphicsOffset;
            graphics = (void *)address0;
            address1 = (u8 *)&marker->primary.sprite;
            primary = (void *)address1;
            address2 = &marker->primary.sprite.priority;
            primaryPriority = address2;
            address0 = (u8 *)&marker->primary.sprite.position;
            primaryPosition = (void *)address0;
            address1 += 0x7C;
            secondaryGraphics = (void *)address1;
            address2 = (u8 *)&marker->secondary.sprite;
            secondary = (void *)address2;
            address0 += 0x3B;
            secondaryPriority = address0;
            address1 -= 0x10;
            secondaryPosition = (void *)address1;
        }
        {
            u32 zero = 0;
            u32 offset;
            u16 *clear;
            i = 1;
            offset = 0x10A;
            clear = (void *)((u8 *)marker + offset);
            do {
                *clear-- = zero;
            } while (--i >= 0);
        }
    }
    {
        u32 enabled;
        enabled = Script_SeekToKeyword('h');
        if (enabled)
            enabled = Script_GetValue();
        marker->bodyEnabled = enabled;
        asm("" ::: "memory");
    }
    if (marker->bodyEnabled) {
        SceneBody_Init(marker->body, marker->id, position, 0, 7, marker);
        TerrainOverlay_QueueAddActor(marker->body);
    }
    paletteBase = &marker->paletteBase;
    *paletteBase = 0xA9;
    {
        register u32 value asm("r1") = 0xBA8;
        asm("" : "+r"(value));
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
    {
        register u32 *source asm("r2") = (u32 *)position;
        u32 lo, hi;
        u32 *destination;
        lo = source[0];
        hi = source[1];
        destination = (u32 *)primaryPosition;
        destination[0] = lo;
        destination[1] = hi;
    }
    AnimData_Load(secondaryGraphics, resourceId);
    {
        struct ContactSwarmSprite *target = secondary;
        Sprite_Init(target, secondaryGraphics, 0);
    }
    secondary->frame = frame;
    {
        u32 tile = *paletteBase;
        AnimData_SelectPaletteBank(secondaryGraphics, tile);
    }
    {
        register struct ContactSwarmSprite *read asm("r1") = secondary;
        u32 flags;
        flags = read->flags | 0x400;
        {
            register struct ContactSwarmSprite *write asm("r2") = secondary;
            write->flags = flags;
        }
    }
    {
        u8 *write = secondaryPriority;
        *write = priority;
    }
    {
        register u32 *source asm("r2") = (u32 *)position;
        u32 lo, hi;
        u32 *destination;
        lo = source[0];
        hi = source[1];
        destination = (u32 *)secondaryPosition;
        destination[0] = lo;
        destination[1] = hi;
    }
    InteractionMarker_SetState(marker, InteractionMarker_TickIdle);
    return 0;
}
