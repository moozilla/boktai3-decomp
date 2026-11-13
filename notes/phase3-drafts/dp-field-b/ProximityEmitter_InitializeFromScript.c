#include "proximity_emitter.h"
s32 ProximityEmitter_InitializeFromScript(struct ProximityEmitter *emitter, u32 unused1,
                                          u32 unused2)
{
    struct ContactPosition *position;
    u32 graphicsId;
    u32 priority;
    u16 *nearby;
    u32 script;
    u32 zero;
    if (Script_SeekToKeyword('p')) {
        u32 cell;
        u32 height, surface;
        u8 *properties;
        struct ContactPosition *point;
        s16 *yAddress;
        s32 x, z;
        {
            u32 value = Script_GetValue();
            point = &emitter->position;
            point->x = value;
        }
        {
            u32 value = Script_GetValue();
            yAddress = &emitter->position.y;
            *yAddress = value;
        }
        emitter->position.z = Script_GetValue();
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
        emitter->position.x = 0;
        emitter->position.y = 0;
        emitter->position.z = 0;
        position = &emitter->position;
    }
    {
        struct ContactSwarmGraphics *graphics;
        struct ContactSwarmSprite *sprite;
    graphics = &emitter->primary.graphics;
    graphicsId = 0xBA8;
    AnimData_Load(graphics, graphicsId);
    sprite = &emitter->primary.sprite;
    Sprite_Init(sprite, graphics, 0);
    zero = 0;
    sprite->frame = zero;
    AnimData_SelectPaletteBank(graphics, 0xA2);
    emitter->primary.sprite.flags |= 0x200;
    {
        u32 value = 3;
        priority = value;
    }
    {
        u32 value = priority;
        emitter->primary.sprite.priority = value;
    }
    {
        struct ContactPosition *source = position;
        emitter->primary.sprite.position = *source;
    }
    emitter->primary.sprite.position.y += 30;
    }
    {
        struct ContactSwarmGraphics *graphics;
        struct ContactSwarmSprite *sprite;
    graphics = &emitter->secondary.graphics;
    AnimData_Load(graphics, graphicsId);
    sprite = &emitter->secondary.sprite;
    Sprite_Init(sprite, graphics, 0);
    sprite->frame = zero;
    AnimData_SelectPaletteBank(graphics, 0xA0);
    emitter->secondary.sprite.flags |= 0x400;
    {
        u8 *destination = &emitter->secondary.sprite.priority;
        u32 value;
        value = priority;
        *destination = value;
    }
    {
        struct ContactPosition *destination = &emitter->secondary.sprite.position;
        struct ContactPosition *source;
        source = position;
        *destination = *source;
    }
    }
    ProximityEmitter_InitializeParticles(emitter);
    if (Script_SeekToKeyword('m') && Script_GetValue()) {
        emitter->primary.sprite.flags |= 1;
        emitter->secondary.sprite.flags |= 1;
        emitter->update = ProximityEmitter_TickGrow;
        Sound_PlaySong(0x15E);
    } else
        emitter->update = ProximityEmitter_TickActive;
    emitter->stateFrame = 0;
    if (ProximityEmitter_IsTetherNearby(emitter)) {
        emitter->nearby = 1;
        nearby = &emitter->nearby;
    } else {
        emitter->nearby = 0;
        nearby = &emitter->nearby;
    }
    script = Script_SeekToKeyword('i');
    if (script)
        emitter->enterScriptId = Script_GetValue();
    else
        emitter->enterScriptId = script;
    script = Script_SeekToKeyword('o');
    if (script)
        emitter->leaveScriptId = Script_GetValue();
    else
        emitter->leaveScriptId = script;
    if (ProximityEmitter_IsTetherNearby(emitter)) {
        u32 value = 1;
        *nearby = value;
        TetherActor_RequestPaletteBlend(value);
    } else {
        *nearby = 0;
        TetherActor_RequestPaletteBlend(0);
    }
    return 0;
}
