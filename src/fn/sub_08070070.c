// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#include "player.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Nav_IsPositionInMesh(void *, const struct ContactPosition *, const struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, const struct ContactPosition *, const struct ContactPosition *);
u8 SceneActorAttachment_TestPositionWindow(struct SceneActorRegistryActor *,
                                           const struct ContactPosition *, u32, u32);
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorRegistryActor *, u32);
struct MovementRouteState {
    u8 unknown0[6];
    u16 field6;
};
/* Mode0 targets two route halfwords; mode1 targets player slot0.
 * Callback sources228,22C,and41C are intentionally distinct. */
void SceneActorRegistry_SetMovementMode(struct SceneActorRegistryActor *input, s32 inputMode)
{
    register struct SceneActorRegistryActor *actor asm("r5") = input;
    register s32 mode asm("r3") = inputMode;
    u8 *group = (u8 *)actor + 0x3d4;
    register u16 *savedFlags asm("r4");
    {
        u32 *flags = &actor->flags2D0;
        u32 mask = ~4;
        u32 value;
        value = *flags;
        value &= mask;
        *flags = value;
    }
    {
        register u32 offset asm("r2") = 0x2d4;
        asm("" : "+r"(offset));
        savedFlags = (void *)((u8 *)actor + offset);
    }
    {
        u32 mask = 0xffffbfff;
        u32 value;
        value = *savedFlags;
        mask &= value;
        *savedFlags = mask;
    }
    if (mode == 0)
        goto modeZero;
    if (mode == 1)
        goto modeOne;
    goto finish;
modeZero: {
    struct ContactPosition target;
    struct ContactPosition *targetPointer;
    u16 *route;
    u32 *cachedFlags;
    u32 *flagsLow;
    s32 cellX, cellZ, index;
    register u8 byteZero asm("r4");
    u32 wordZero;
    u8 one;
    register u32 controlOffset asm("r6");
    u8 *cell;
    u32 kind, height;
    void *routeObject;
    s32 result;
    const void *control;
    {
        u32 mask = 0x10;
        register u32 offset asm("r6") = 0x2cc;
        register u32 *flags asm("r3");
        u32 value;
        asm("" : "+r"(offset) : "r"(mask));
        flags = (void *)((u8 *)actor + offset);
        value = *flags;
        value |= mask;
        *flags = value;
        flagsLow = flags;
    }
    targetPointer = &target;
    {
        u32 offset = 0x448;
        u8 *base;
        register u32 sample asm("r1");
        base = (u8 *)actor + offset;
        route = *(u16 **)(base + 8);
        sample = route[0];
        target.x = sample;
        sample = route[1];
        targetPointer->z = sample;
        cellX = target.x >> 8;
        cellZ = (s16)sample >> 8;
    }
    cachedFlags = flagsLow;

    if (cellX < 0 || cellZ < 0 || (u32)cellX >= gTerrainWidth || (u32)cellZ >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[cellZ] + cellX;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        cell = map->cells + offset;
    }
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto slopeDone;
slopeZ:
    height -= *((u8 *)targetPointer + 4);
    goto slopeDone;
slopeX:
    height -= *(u8 *)targetPointer;
slopeDone:;
    /* The terrain probe is retained, then actorY overrides its height. */
    targetPointer->y = height;
    target.y = actor->position.y;
    routeObject = *(void **)(group + 0x1fc);
    {
        struct ContactPosition *position = &actor->position;
        register u16 *flagsToClear asm("r0");
        result = Nav_IsPositionInMesh(routeObject, position, &target);
        if (!result) {
            register u32 offset asm("r2") = 0x2d4;
            u16 *flags;
            asm("" : "+r"(offset));
            flags = (void *)((u8 *)actor + offset);
            flagsToClear = flags;
            goto clearRoute;
        }
        result = Terrain_ApplyBlocked(routeObject, position, &target);
        if (!result) {
            register u32 offset asm("r6") = 0x2d4;
            u16 *flags;
            asm("" : "+r"(offset));
            flags = (void *)((u8 *)actor + offset);
            flagsToClear = flags;
            goto clearRoute;
        }
        if (result != 2)
            Memory_Clear(&actor->velocity, 8);
        goto routeReady;
    clearRoute: {
        u16 *flags = flagsToClear;
        u32 mask = 0xfffffdff;
        u32 value;
        value = *flags;
        mask &= value;
        *flags = mask;
    }
    routeReady:;
    }
    {
        struct MovementRouteState *phase = *(struct MovementRouteState **)(group + 0x200);
        byteZero = 0;
        wordZero = 0;
        one = 1;
        {
            register u32 initial asm("r0") = 1;
            phase->field6 = initial;
        }
    }
    if (!SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
        {
            u32 mask = ~0xc0;
            register u32 *flags asm("r2") = cachedFlags;
            u32 value;
            value = *flags;
            value &= mask;
            *flags = value;
        }
        {
            u32 behavior = 7;
            u32 offset = 0x2a8;
            register u8 *field asm("r0");
            register u32 counterOffset asm("r1");
            field = (u8 *)actor + offset;
            *field = byteZero;
            offset++;
            field = (u8 *)actor + offset;
            *field = byteZero;
            offset++;
            field = (u8 *)actor + offset;
            *field = behavior;
            counterOffset = 0x2ac;
            field = (u8 *)actor + counterOffset;
            *(u32 *)field = wordZero;
            offset += 7;
            field = (u8 *)actor + offset;
            *field = one;
            /* Non-near route path restores pointer228. */
            counterOffset -= 0x84;
            field = (u8 *)actor + counterOffset;
            control = *(const void **)field;
            offset++;
            field = (u8 *)actor + offset;
            *field = one;
            offset -= 2;
            field = (u8 *)actor + offset;
            *field = byteZero;
            offset += 5;
            field = (u8 *)actor + offset;
            *field = one;
            controlOffset = 0x21c;
        }
    } else {
        {
            register u32 mask asm("r1") = ~0xd0;
            register u32 *flags asm("r2") = cachedFlags;
            u32 value;
            value = *flags;
            value &= mask;
            *flags = value;
        }
        {
            u32 offset = 0x2a8;
            register u32 phaseOffset asm("r2");
            register u8 *field asm("r0");
            field = (u8 *)actor + offset;
            *field = byteZero;
            phaseOffset = 0x2a9;
            field = (u8 *)actor + phaseOffset;
            *field = byteZero;
            offset += 2;
            field = (u8 *)actor + offset;
            *field = byteZero;
            phaseOffset += 3;
            field = (u8 *)actor + phaseOffset;
            *(u32 *)field = wordZero;
            offset += 7;
            field = (u8 *)actor + offset;
            *field = one;
            /* Near route path restores pointer22C. */
            phaseOffset -= 0x80;
            field = (u8 *)actor + phaseOffset;
            control = *(const void **)field;
            phaseOffset += 0x86;
            field = (u8 *)actor + phaseOffset;
            *field = one;
            controlOffset = 0x2b0;
            field = (u8 *)actor + controlOffset;
            *field = byteZero;
            phaseOffset += 3;
            field = (u8 *)actor + phaseOffset;
            *field = byteZero;
            controlOffset -= 0x94;
        }
    }
    {
        u8 *field = (u8 *)actor + controlOffset;
        *(const void **)field = control;
        {
            u32 offset = 0x2c8;
            field = (u8 *)actor + offset;
            *(u16 *)field = wordZero;
        }
        {
            register u32 offset asm("r6") = 0x114;
            register u32 *flags asm("r2");
            u32 mask;
            u32 value;
            asm("" : "+r"(offset));
            flags = (void *)((u8 *)actor + offset);
            mask = ~1;
            value = *flags;
            value &= mask;
            *flags = value;
        }
        {
            u32 offset = 0x104;
            field = (u8 *)actor + offset;
            *(u32 *)field = wordZero;
        }
        {
            register u32 offset asm("r2") = 0x2c2;
            register u16 *destination asm("r1");
            register u32 value asm("r0");
            asm("" : "+r"(offset));
            destination = (void *)((u8 *)actor + offset);
            value = 64;
            *destination = value;
        }
    }
}
    goto finish;
modeOne: {
    void *routeObject;
    const struct ContactPosition *target;
    struct ContactPosition *position;
    s32 result;
    {
        register u32 offset asm("r6") = 0x414;
        register u16 *field asm("r2");
        u32 value;
        u32 mask;
        asm("" : "+r"(offset));
        field = (void *)((u8 *)actor + offset);
        mask = 4;
        value = *field;
        mask |= value;
        *field = mask;
    }
    {
        u32 mask = 0x80;
        u32 value;
        value = *savedFlags;
        mask |= value;
        *savedFlags = mask;
    }
    {
        u32 readOffset = 0x416;
        u16 *field;
        register u32 value asm("r1");
        register u32 writeOffset asm("r2");
        field = (void *)((u8 *)actor + readOffset);
        value = *field;
        writeOffset = 0x418;
        asm("" : "+r"(writeOffset));
        field = (void *)((u8 *)actor + writeOffset);
        *field = value;
    }
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        struct Player **players = gUnk_02000580;
        register u32 offset asm("r6") = 0x5d0;
        register void **field asm("r0");
        asm("" : "+r"(offset) : "r"(players));
        field = (void *)((u8 *)actor + offset);
        routeObject = *field;
        position = &actor->position;
        {
            struct Player *current = players[0];
            target = (const struct ContactPosition *)&current->position;
        }
    }
    result = Nav_IsPositionInMesh(routeObject, position, target);
    if (!result)
        goto clearPlayerRoute;
    result = Terrain_ApplyBlocked(routeObject, position, target);
    if (!result)
        goto clearPlayerRoute;
    if (result == 2)
        goto playerRouteReady;
    Memory_Clear(&actor->velocity, 8);
    goto playerRouteReady;
clearPlayerRoute: {
    u32 mask = 0xfffffdff;
    u32 value = *savedFlags;
    mask &= value;
    *savedFlags = mask;
}
playerRouteReady:;
    {
        u32 behavior = 12;
        register u32 offset asm("r2") = 0x2a8;
        register u8 *field asm("r0");
        u32 zero;
        register u32 phaseOffset asm("r6");
        const void *control;
        field = (u8 *)actor + offset;
        zero = 0;
        *field = zero;
        phaseOffset = 0x2a9;
        field = (u8 *)actor + phaseOffset;
        *field = zero;
        offset += 2;
        asm("" : "+r"(offset));
        field = (u8 *)actor + offset;
        *field = behavior;
        phaseOffset += 3;
        field = (u8 *)actor + phaseOffset;
        *(u32 *)field = zero;
        {
            u32 entryOffset = 0x2b1;
            u32 one;
            field = (u8 *)actor + entryOffset;
            one = 1;
            *field = one;
            {
                register u8 *owner asm("r2") = group;
                asm("" : "+r"(owner));
                control = *(const void **)(owner + 0x48);
            }
            {
                u32 code = 26;
                phaseOffset += 6;
                field = (u8 *)actor + phaseOffset;
                *field = one;
                {
                    u32 beginOffset = 0x2b0;
                    field = (u8 *)actor + beginOffset;
                    *field = zero;
                    phaseOffset += 3;
                    field = (u8 *)actor + phaseOffset;
                    *field = code;
                    beginOffset -= 0x94;
                    field = (u8 *)actor + beginOffset;
                    *(const void **)field = control;
                }
                {
                    register u32 counterOffset asm("r2") = 0x2c8;
                    asm("" : "+r"(counterOffset));
                    field = (u8 *)actor + counterOffset;
                    *(u16 *)field = zero;
                }
                {
                    register u32 flagOffset asm("r6") = 0x114;
                    register u32 *flags asm("r2");
                    u32 mask;
                    u32 value;
                    asm("" : "+r"(flagOffset));
                    flags = (void *)((u8 *)actor + flagOffset);
                    mask = ~1;
                    value = *flags;
                    value &= mask;
                    *flags = value;
                }
                {
                    u32 clearOffset = 0x104;
                    field = (u8 *)actor + clearOffset;
                    *(u32 *)field = zero;
                }
            }
        }
    }
}
finish:
    return;
}
