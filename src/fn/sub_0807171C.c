// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
void SceneActorRegistry_ResetRoute(struct SceneActorRegistryActor *argument)
{
    struct SceneActorRegistryResetView *actor = (void *)argument;
    struct SceneActorRegistryControlGroup *group = &actor->group;
    struct SceneActorRegistryRoute *route = &actor->route;
    struct ContactPosition *position;
    struct SceneActorRegistryControl *control;
    u32 *flags, *renderFlags;
    s32 x, z, cell, height, slope;
    s32 computedCell;
    u8 *data;
    route->fieldC = route->parameterC;
    Route_Start(route, route->parameterA, route->parameterB, route->parameterC);
    {
        u32 offset = 0x5D0;
        void *object;
        object = *(void **)((u8 *)actor + offset);
        position = &actor->body.position;
        ProximityVolume_TranslateBounds(object, position);
    }
    actor->body.currentRoute = route;
    x = position->x >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        computedCell = 0;
    else {
        u32 row = gTerrainMap->rows[z];
        computedCell = row + x;
    }
    {
        u32 offset = 0x2F4;
        u16 *address;
        address = (u16 *)((u8 *)actor + offset);
        *address = computedCell;
        cell = *address;
    }
    position = &actor->body.position;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + 12 + cell * 4;
    slope = *data >> 4;
    height = (*data & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->body.position.y = height;
    if (!actor->drawableKind)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position = actor->body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->body.position;
    {
        u16 *address = &actor->field2EA;
        register u32 zero asm("r5");
        zero = 0;
        *address = zero;
        {
            u32 offset = 0x2EC;
            Memory_Clear((u8 *)actor + offset, 8);
        }
        Memory_Clear(&actor->body.motion, 8);
        control = &group->control;
        {
            u32 mask = 0x200000;
            u32 *address;
            u32 selected;
            address = &actor->flags2CC;
            selected = *address & mask;
            flags = address;
            renderFlags = &actor->flags2D0;
            if (selected) {
                u32 clearMask = ~0x1000000;
                u32 value;
                u16 *shortFlags;
                u32 offset;
                u32 mask0;
                value = *renderFlags;
                value &= clearMask;
                *renderFlags = value;
                offset = 0x2D4;
                shortFlags = (u16 *)((u8 *)actor + offset);
                mask0 = 0xFFFFFDFF;
                mask0 &= *shortFlags;
                *shortFlags = mask0;
                control->mode = 0;
                control->counter = zero;
                control->timer = zero;
            }
        }
    }
    {
        register u32 fieldOffset asm("r1") = 0x1E0;
        const u16 *base;
        const u16 *offset;
        register u32 bank asm("r1");
        asm("" : "+r"(fieldOffset));
        base = (const u16 *)((u8 *)actor + fieldOffset);
        fieldOffset += 6;
        offset = (const u16 *)((u8 *)actor + fieldOffset);
        bank = *base;
        asm("" : "+r"(bank));
        bank += *offset;
        if (!actor->drawableKind)
            AnimData_SelectPaletteBank(
                ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->animation, bank);
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
                gPaletteBankData + (bank << 5);
    }
    {
        u32 mask = 0xFDFF7FFF;
        *flags &= mask;
        mask = 0xFFFFBE3F;
        *renderFlags &= mask;
    }
    {
        u32 offset = 0x1EA;
        register u32 zero asm("r1");
        register u16 *address asm("r0");
        address = (u16 *)((u8 *)actor + offset);
        zero = 0;
        *address = zero;
        offset -= 2;
        address = (u16 *)((u8 *)actor + offset);
        *address = zero;
    }
    {
        u32 mode = 2;
        if (!actor->drawableKind)
            ((u8 *)actor->drawable)[7] = mode;
        else
            ((u8 *)actor->drawable)[0x3A] = mode;
    }
}
