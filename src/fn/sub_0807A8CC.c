#include "global.h"
#ifdef NONMATCHING
// fuzzy: 99.5%
// Complete reviewed 1736-byte tick body; health/mode reload allocation remains.
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_motion.h"
#include "game_state.h"
#include "repeated_effect.h"
void SceneActorMovement_UpdateAirborne(struct SceneActorMotionView *);
void SceneActorMovement_UpdateHeight(struct SceneActorRegistryActor *);
struct SceneActorRegistryResetView;
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);
void Obj_StepPosition(void *);
void NavMesh_UpdateProbe(u16 *, u32);
void sub_0821DE30(struct SceneActorRegistryControlGroup *);
static inline u32 ReadMotionFlags(struct SceneActorMotionView *actor, u32 mask)
{
    register u32 offset asm("r6") = (u8 *)&actor->flags2D0 - (u8 *)actor;
    register u32 *field asm("r0");
    asm("" : "+r"(offset));
    field = (void *)((u8 *)actor + offset);
    return *field & mask;
}
static inline u8 IsUnavailable(struct SceneActorMotionView *argument)
{
    register struct SceneActorMotionView *actor asm("r5") = argument;
    if (actor) {
        u32 flags = actor->flags2D4;
        flags &= 0x40;
        if (flags || SceneActorPlacement_ReadFlags32((void *)actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 HasGameFlag(u32 mask, u32 *savedOffset)
{
    register struct GameState *game = gGameState;
    register u32 offset asm("r2");
    register u32 *flags;
    offset = (u8 *)&game->flags868 - (u8 *)game;
    flags = (void *)((u8 *)game + offset);
    *savedOffset = offset;
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u32 StepSequence(struct SceneActorMotionSequence *argument, u32 one)
{
    register struct SceneActorMotionSequence *sprite = argument;
    register struct DisplaySequenceState *state = &sprite->sequence;
    const u16 *command;
    u32 input, ended, flags;
    register u32 first asm("r1");
    register u32 bit asm("r3");
    command = sprite->sequence.commands + state->index;
    sprite->frame = *command >> 6;
    input = state->flags;
    bit = one;
    first = bit;
    first &= input;
    if (first != (((*command & 0x30) >> 4) & bit))
        sprite->flags |= 4;
    else
        sprite->flags &= ~4;
    input = state->flags;
    bit = 2;
    first = bit;
    first &= input;
    first = (u8)first;
    if (first != (((*command & 0x30) >> 4) & bit))
        sprite->flags |= 8;
    else
        sprite->flags &= ~8;
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        u32 duration;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        {
            register u32 temp asm("r1") = 65535;
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        }
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (!index) {
                    register u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 count;
                state->index = index;
                count = state->frameCount;
                if ((index & mask) >= count) {
                    register u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        state->elapsed = duration;
                        state->index = count - 1;
                    } else
                        state->index = 0;
                    ended = 1;
                } else
                    ended = 0;
            }
            command = state->commands + state->index;
            {
                register u32 weight asm("r1") = *command;
                state->frameWeight = weight & 15;
            }
            {
                s32 weight = state->frameWeight;
                s32 speed = state->speed;
                s32 delay = (speed * weight) >> 6;
                state->duration = delay;
                if (!(u8)delay)
                    state->duration = 1;
            }
        } else
            ended = 0;
    }
    return ended;
}
struct TileState {
    u8 unknown0[0x1D];
    u8 ended;
};
struct SceneActorMotionCache {
    struct ContactPosition center;
    struct ContactPosition difference;
    struct SceneActorMotionTerrainGroup *group;
    struct ContactCollider *primary;
    u32 *flags;
    u16 *motionFlags;
    u32 animationOne;
};
s32 SceneActorMotion_Update(struct SceneActorMotionView *argument)
{
    register struct SceneActorMotionView *actor asm("r5") = argument;
    struct SceneActorMotionCache cache;
    struct ContactPosition *position;
    register u32 motionFlagOffset;
    register u32 motionMask;
    {
        u8 *enabled;
        register u32 one asm("r3");
        register u8 *endedField;
        {
            register u32 offset = (u8 *)&actor->group - (u8 *)actor;
            register struct SceneActorRegistryControlGroup *group;
            asm("" : "+r"(offset));
            group = (void *)((u8 *)actor + offset);
            cache.group = (void *)group;
        }
        enabled = &actor->enabled;
        one = 1;
        *enabled = one;
        {
            register u32 offset = (u8 *)&actor->flags2CC - (u8 *)actor;
            u32 value;
            register u32 *field;
            field = (void *)((u8 *)actor + offset);
            value = *field;
            if (value & one) {
                register u32 mask;
                register u16 *flags asm("r2");
                register u32 original asm("r1");
                register u32 fieldOffset asm("r4");
                mask = 0x40;
                fieldOffset = ((u8 *)&actor->flags2D4 - (u8 *)actor);
                asm("" : "+r"(fieldOffset));
                flags = (u16 *)((u8 *)actor + fieldOffset);
                original = *flags;
                mask |= original;
                *flags = mask;
            }
        }
        {
            register u32 fieldOffset = ((u8 *)&actor->animationEnded - (u8 *)actor);
            asm("" : "+r"(fieldOffset));
            endedField = (u8 *)actor + fieldOffset;
        }
        if (*endedField != 1) {
            if (!actor->drawableKind) {
                register void *drawable asm("r0") = actor->drawable;
                asm("" : "+r"(drawable));
                actor->animationEnded = StepSequence(drawable, one);
            } else {

                register void *raw = actor->drawable;
                register struct TileState *tile = (void *)((u8 *)raw + 0x20);
                register void *argument asm("r0") = tile;
                register u32 ended;
                register u32 status asm("r1");
                cache.animationOne = one;
                ended = Anim_Step(argument, raw);
                status = tile->ended;
                one = cache.animationOne;
                {
                    register u32 masked = one;
                    masked &= status;
                    if (masked || ended)
                        *endedField = one;
                }
            }
        }
    }
    {
        register u32 unavailable;
        if (actor) {
            u32 mask = 0x40;
            register u32 offset asm("r4") = (u8 *)&actor->flags2D4 - (u8 *)actor;
            register u16 *flags asm("r0");
            asm("" : "+r"(offset));
            flags = (void *)((u8 *)actor + offset);
            if ((*flags & mask) || ReadMotionFlags(actor, 0x20000) || actor->health <= 0)
                unavailable = 1;
            else
                unavailable = 0;
        } else
            unavailable = 0;
        asm("" : "+r"(unavailable));
        if (unavailable)
            return 1;
    }
    SceneActorMovement_UpdateAirborne(actor);
    {

        register u32 *flags;
        register u32 offset;
        register u32 moving;
        motionMask = 0x2000;
        motionFlagOffset = (u8 *)&actor->flags2CC - (u8 *)actor;
        flags = (void *)((u8 *)actor + motionFlagOffset);
        moving = *flags & motionMask;
        cache.flags = flags;
        {
            register void *map asm("r0");
            if (moving) {
                register u32 blockedMask = 0x20;
                u8 blocked;
                {
                    register u32 offset asm("r4") = (u8 *)&actor->flags2C0 - (u8 *)actor;
                    register u16 *field asm("r0");
                    asm("" : "+r"(offset));
                    field = (void *)((u8 *)actor + offset);
                    asm("" : "+r"(field));
                    blocked = SceneActorPlacement_TestFlags(field, blockedMask);
                }
                if (!blocked)
                    map = (u8 *)actor + 0x40;
                else
                    map = 0;
            } else
                map = 0;
            actor->motionMap = map;
        }
    }
    {
        s32 moving;
        {
            register s16 *field = &actor->velocity.x;
            moving = *field;
        }
        if (!moving) {
            register s16 *field = &actor->velocity.z;
            moving = *field;
        }
        if (moving) {
            register u32 mask = 0x2000;
            register u32 offset = (u8 *)&actor->flags2D6 - (u8 *)actor;
            register u16 *motionFlags;
            register u32 previous;
            motionFlags = (void *)((u8 *)actor + offset);
            previous = *motionFlags;
            mask |= previous;
            *motionFlags = mask;
            cache.motionFlags = motionFlags;
            {
                register u32 offset = (u8 *)&actor->position - (u8 *)actor;
                register struct ContactPosition *current;
                current = (void *)(offset + (u32)actor);
                position = current;
            }
        } else {
            register u32 offset = (u8 *)&actor->previousPosition - (u8 *)actor;
            register s16 *old;
            register s32 oldX;
            register struct ContactPosition *current;
            register s32 currentX;
            register u16 *motionFlags asm("r4");
            old = (void *)((u8 *)actor + offset);
            oldX = *old;
            asm("" : : "r"(motionFlagOffset));
            current = &actor->position;
            currentX = current->x;
            offset += (u8 *)&actor->flags2D6 - (u8 *)&actor->previousPosition;
            motionFlags = (void *)((u8 *)actor + offset);
            cache.motionFlags = motionFlags;
            position = current;
            if (oldX == currentX) {
                register u32 offset = (u8 *)&actor->previousPosition.z - (u8 *)actor;
                register s16 *previousZ;
                register s32 oldZ;
                previousZ = (void *)((u8 *)actor + offset);
                oldZ = *previousZ;
                if (oldZ == actor->position.z) {
                    register u32 mask = 0x2000;
                    register u32 previous;
                    previous = *motionFlags;
                    mask |= previous;
                    *motionFlags = mask;
                }
            }
        }
    }
    {
        register u32 mask = 0x2000;
        register u16 *motionFlags asm("r4");
        motionFlags = cache.motionFlags;
        asm("" : "+r"(motionFlags));
        if (SceneActorPlacement_TestFlags(motionFlags, mask))
            Obj_StepPosition(&actor->id);
    }
    {
        register u32 offset asm("r6") = (u8 *)&actor->terrainCell - (u8 *)actor;
        register u16 *cell asm("r0");
        register u32 current;
        register u32 previousOffset asm("r2");
        asm("" : "+r"(offset));
        cell = (void *)((u8 *)actor + offset);
        current = *cell;
        previousOffset = (u8 *)&actor->previousTerrainCell - (u8 *)actor;
        asm("" : "+r"(previousOffset));
        {
            register u16 *previous asm("r0") = (void *)((u8 *)actor + previousOffset);
            *previous = current;
        }
    }
    {
        register const struct ContactPosition *current asm("r3") = position;
        register u32 cell;
        asm("" : "+r"(current));
        cell = SceneActorTerrain_GetCellIndex(current);
        {
            register u32 offset = (u8 *)&actor->terrainCell - (u8 *)actor;
            {
                register u16 *field = (void *)((u8 *)actor + offset);
                *field = cell;
            }
        }
    }
    SceneActorMovement_UpdateHeight((void *)actor);
    {
        register u32 mask = 0x2000;
        register u16 *motionFlags asm("r6");
        motionFlags = cache.motionFlags;
        asm("" : "+r"(motionFlags));
        if (SceneActorPlacement_TestFlags(motionFlags, mask)) {
            u32 category = actor->category;
            u32 excluded;
            if ((category >= 17 && category <= 20) || category == 23)
                excluded = 1;
            else
                excluded = 0;
            asm("" : "+r"(excluded));
            if (!excluded) {
                u16 *cellIndex;
                u8 *cellData;
                u16 cellFlags;
                {
                    register u32 offset = (u8 *)&actor->terrainCell - (u8 *)actor;
                    asm("" : "+r"(offset));
                    cellIndex = (void *)((u8 *)actor + offset);
                }
                {
                    register u32 cell asm("r6") = *cellIndex;
                    cellData = TerrainModifiers_FindCellOverride(cell, 1);
                    if (cellData)
                        cellData += 4;
                    else {
                        register struct EngineMap *map = gTerrainMap;
                        register u32 offset asm("r0");
                        offset = cell * 4 + 12;
                        cellData = map->cells + offset;
                    }
                }
                cellFlags = *(u16 *)(cellData + 2);
                {
                    register u32 offset asm("r3") = (u8 *)&actor->previousTerrainCell - (u8 *)actor;
                    register u16 *previous asm("r1");
                    asm("" : "+r"(offset));
                    previous = (void *)((u8 *)actor + offset);
                    if (*cellIndex != *previous) {
                        if ((cellFlags & 0x40) &&
                            actor->position.y + actor->verticalOffset <= actor->groundHeight) {
                            register u32 unavailable;
                            {
                                register u32 result;
                                result = IsUnavailable(actor);
                                unavailable = result;
                            }
                            if (!unavailable) {
                                register const void *control;
                                register u32 offset asm("r6") =
                                    (u8 *)&actor->terrainControl - (u8 *)actor;
                                register const void **field;
                                register u32 kind asm("r2");
                                register u32 substateOffset asm("r1");
                                field = (void *)((u8 *)actor + offset);
                                control = *field;
                                if (!control) {
                                    control = SceneActorReaction_SinkAndRemove;
                                    kind = 22;
                                    {
                                        register u32 preparedOffset =
                                            (u8 *)&actor->prepared - (u8 *)actor;
                                        register u8 *prepared;
                                        prepared = (void *)((u8 *)actor + preparedOffset);
                                        *prepared = 1;
                                    }
                                    {
                                        register u8 *substate asm("r0");
                                        substateOffset = (u8 *)&actor->substate - (u8 *)actor;
                                        substate = (void *)((u8 *)actor + substateOffset);
                                        *substate = unavailable;
                                    }
                                    offset += (u8 *)&actor->kind - (u8 *)&actor->terrainControl;
                                } else {
                                    kind = 22;
                                    {
                                        register u32 preparedOffset asm("r0") =
                                            (u8 *)&actor->prepared - (u8 *)actor;
                                        register u8 *prepared asm("r1");
                                        asm("" : "+r"(preparedOffset));
                                        prepared = (void *)((u8 *)actor + preparedOffset);
                                        *prepared = 1;
                                    }
                                    {
                                        register u8 *substate;
                                        substateOffset = (u8 *)&actor->substate - (u8 *)actor;
                                        substate = (void *)((u8 *)actor + substateOffset);
                                        *substate = unavailable;
                                    }
                                    offset = (u8 *)&actor->kind - (u8 *)actor;
                                }
                                {
                                    register u8 *field asm("r0") = (void *)((u8 *)actor + offset);
                                    *field = kind;
                                }
                                {
                                    register const void **field asm("r0");
                                    substateOffset +=
                                        (u8 *)&actor->control - (u8 *)&actor->substate;
                                    field = (void *)((u8 *)actor + substateOffset);
                                    *field = control;
                                }
                                {
                                    register u32 offset asm("r2") =
                                        (u8 *)&actor->field2C8 - (u8 *)actor;
                                    register u16 *field asm("r0");
                                    asm("" : "+r"(offset));
                                    field = (void *)((u8 *)actor + offset);
                                    *field = unavailable;
                                }
                                {
                                    register u32 *flags asm("r2");
                                    register u32 offset asm("r3") =
                                        (u8 *)&actor->flags114 - (u8 *)actor;
                                    register u32 mask;
                                    asm("" : "+r"(offset));
                                    flags = (void *)((u8 *)actor + offset);
                                    mask = ~1;
                                    *flags &= mask;
                                }
                                {
                                    register u32 offset asm("r6") =
                                        (u8 *)&actor->field104 - (u8 *)actor;
                                    register u32 *field asm("r0");
                                    asm("" : "+r"(offset));
                                    field = (void *)((u8 *)actor + offset);
                                    *field = unavailable;
                                }
                                {
                                    register u32 stateValue = 3;
                                    register u8 *state;
                                    register u32 offset = (u8 *)&actor->state - (u8 *)actor;
                                    register u32 zero;
                                    state = (void *)((u8 *)actor + offset);
                                    zero = 0;
                                    *state = stateValue;
                                    {
                                        register u32 offset = (u8 *)&actor->phase - (u8 *)actor;
                                        register u8 *field;
                                        field = (void *)((u8 *)actor + offset);
                                        *field = zero;
                                    }
                                    {
                                        register u32 offset = (u8 *)&actor->behavior - (u8 *)actor;
                                        register u8 *field;
                                        field = (void *)((u8 *)actor + offset);
                                        *field = zero;
                                    }
                                    {
                                        register u32 offset = (u8 *)&actor->counter - (u8 *)actor;
                                        register u32 *field;
                                        field = (void *)((u8 *)actor + offset);
                                        *field = zero;
                                    }
                                }
                                {
                                    register u32 offset =
                                        (u8 *)&actor->resetRequested - (u8 *)actor;
                                    register u8 *field;
                                    field = (void *)((u8 *)actor + offset);
                                    *field = 1;
                                }
                                {
                                    register u32 mask = 0x20000;
                                    register u32 *flags;
                                    register u32 offset = (u8 *)&actor->flags2D0 - (u8 *)actor;
                                    flags = (void *)((u8 *)actor + offset);
                                    *flags |= mask;
                                }
                            }
                            goto refresh;
                        }
                        if (cellFlags & 0x80) {
                            register u32 mask = 0x10;
                            u8 hasFlag;
                            u32 savedOffset;
                            register u32 gameOffset;
                            register struct ContactCollider *primary asm("r3");
                            register s32 savedZ;
                            hasFlag = HasGameFlag(mask, &savedOffset);
                            gameOffset = savedOffset;
                            if (!hasFlag)
                                goto ordinaryMotion;

                            {
                                register u32 offset = (u8 *)&actor->primaryCollider - (u8 *)actor;
                                asm("" : "+r"(offset));
                                primary = (void *)((u8 *)actor + offset);
                                cache.primary = primary;
                            }
                            {
                                register struct ContactPosition *center;
                                register u32 cell = *cellIndex;
                                struct EngineMap **map;
                                register s32 x;
                                register s32 z;
                                register u32 attributes asm("r1");
                                center = &cache.center;
                                map = &gTerrainMap;
                                {
                                    register s32 width = *(s16 *)((*map)->cells + 4);
                                    asm("" : "+r"(width) : "r"(gameOffset), "r"(primary));
                                    x = Mod(cell, width);
                                }
                                z = Div(cell, *(s16 *)((*map)->cells + 4));
                                x <<= 8;
                                x += 128;
                                cache.center.x = x;
                                {
                                    u8 *record = TerrainModifiers_FindCellOverride(cell, 1);
                                    if (record)
                                        record += 4;
                                    else {
                                        struct EngineMap *currentMap = *map;
                                        register u32 byteOffset;
                                        byteOffset = cell * 4 + 12;
                                        record = currentMap->cells + byteOffset;
                                    }
                                    attributes = *record;
                                }
                                {
                                    register struct ContactPosition *localCenter asm("r3");
                                    register s32 height = (attributes & 15) << 8;
                                    localCenter = center;
                                    localCenter->y = height;
                                    savedZ = z;
                                    {
                                        register s32 centeredZ = ((u32)savedZ << 8) + 128;
                                        localCenter->z = centeredZ;
                                    }
                                }
                            }
                            {
                                register struct ContactPosition *difference = &cache.difference;
                                register const struct ContactPosition *center = &cache.center;
                                register const struct ContactPosition *current asm("r6");
                                {
                                    register u32 centerX = (u16)cache.center.x;
                                    current = position;
                                    difference->x = centerX - (u16)current->x;
                                    difference->y = center->y - current->y;
                                    difference->z = center->z - current->z;
                                }
                                {
                                    register s32 firstCoordinate = difference->x;
                                    register s32 direction;
                                    register u32 offset;
                                    register u8 *field;
                                    asm(""
                                        : "+r"(firstCoordinate)
                                        : "r"(savedZ), "r"(center), "r"(current));
                                    direction = Math_Atan2(firstCoordinate, difference->z) >> 5;
                                    offset = (u8 *)&actor->effectDirection - (u8 *)actor;
                                    field = (void *)((u8 *)actor + offset);
                                    *field = direction;
                                }
                            }
                            {
                                register u32 offset asm("r6") =
                                    (u8 *)&actor->payload2E0 - (u8 *)actor;
                                register u32 *payload;
                                register s16 *health;
                                register s32 firstHealth;
                                register struct ContactCollider *primary asm("r3");
                                asm("" : "+r"(offset));
                                payload = (void *)((u8 *)actor + offset);
                                *payload = 32;
                                health = &actor->health;
                                firstHealth = *health;
                                primary = cache.primary;
                                primary->cost = firstHealth;
                                {
                                    register s32 secondHealth = *health;
                                    register u16 *result asm("r0");
                                    offset += (u8 *)&actor->result2E4 - (u8 *)&actor->payload2E0;
                                    result = (void *)((u8 *)actor + offset);
                                    *result = secondHealth;
                                }
                                {
                                    register u16 *field asm("r1") = (void *)((u8 *)primary + 0x40);
                                    *field = 40;
                                }
                            }
                            {
                                register u32 offset = (u8 *)&actor->parameter2D8 - (u8 *)actor;
                                register s32 *field;
                                register u32 strength asm("r0");
                                field = (void *)((u8 *)actor + offset);
                                asm("" : "+r"(field) : "r"(offset));
                                strength = 512;
                                *field = strength;
                            }
                            {
                                register u32 offset = (u8 *)&actor->mode2E6 - (u8 *)actor;
                                register u16 *field;
                                field = (void *)((u8 *)actor + offset);
                                *field = 1;
                            }
                            {
                                register u32 mask = 0x2000;
                                register u32 *flags asm("r3");
                                flags = cache.flags;
                                *flags |= mask;
                            }
                            goto refresh;
                        }
                    }
                }
            ordinaryMotion:
                if (category == 3) {
                    register u32 offset asm("r4") = (u8 *)&actor->movementTicks - (u8 *)actor;
                    register u16 *ticks asm("r0");
                    asm("" : "+r"(offset));
                    ticks = (void *)((u8 *)actor + offset);
                    if (*ticks & 1) {
                        register u32 mask = 0x100;
                        register u32 offset asm("r6") = (u8 *)&actor->flags2D4 - (u8 *)actor;
                        register u16 *flags asm("r0");
                        asm("" : "+r"(mask), "+r"(offset));
                        flags = (void *)((u8 *)actor + offset);
                        if (*flags & mask) {
                            register u32 addMask = 0x10000;
                            register u32 offset asm("r0") = (u8 *)&actor->flags2D0 - (u8 *)actor;
                            register u32 *field asm("r1");
                            field = (void *)((u8 *)actor + offset);
                            *field |= addMask;
                        }
                    }
                }
                {
                    u32 elapsed;
                    if (!actor->drawableKind) {
                        register struct DisplaySequenceState *sequence =
                            &((struct SceneActorMotionSequence *)actor->drawable)->sequence;
                        elapsed = sequence->elapsed;
                    } else
                        elapsed = ((struct SceneActorMotionTile *)actor->drawable)->elapsed;
                    if (!elapsed) {
                        register u32 mask = 0x10000;
                        register u32 offset asm("r2") = (u8 *)&actor->flags2D0 - (u8 *)actor;
                        register u32 *flags;
                        flags = (void *)((u8 *)actor + offset);
                        if (*flags & mask) {
                            register u32 clearMask;
                            RepeatedEffect_RequestNearby((void *)position);
                            clearMask = ~0x10000;
                            *flags &= clearMask;
                        }
                    }
                }
            }
        }
    }
refresh: {
    register struct SceneActorMotionTerrainGroup *group asm("r3") = cache.group;
    register u32 offset asm("r4") = (u8 *)&group->contact - (u8 *)group;
    register void *contact;
    register u32 mask;
    register u32 *flags asm("r2");
    {
        register void **field asm("r0") = (void *)((u8 *)group + offset);
        contact = *field;
    }
    mask = 0x400000;
    flags = cache.flags;
    if (*flags & mask) {
        register u32 offset asm("r3") = (u8 *)&actor->terrainCell - (u8 *)actor;
        u16 cellFlags;
        asm("" : "+r"(offset));
        {
            register u16 *field asm("r0") = (void *)((u8 *)actor + offset);
            cellFlags = *(u16 *)(SceneActorTerrain_GetCell(*field) + 2);
        }
        if ((cellFlags & 3) != 3 && !(cellFlags & 0x440))
            NavMesh_UpdateProbe(contact, (u32)position);
    }
}
    {
        register u32 offset asm("r4") = (u8 *)&actor->movementTicks - (u8 *)actor;
        register u16 *ticks asm("r1");
        asm("" : "+r"(offset));
        ticks = (void *)((u8 *)actor + offset);
        ++*ticks;
    }
    sub_0821DE30((void *)cache.group);
    {
        register u32 offset asm("r6") = (u8 *)&actor->previousPosition - (u8 *)actor;
        register struct ContactPosition *previous asm("r2");
        previous = (void *)((u8 *)actor + offset);
        *previous = actor->position;
    }
    {
        register u32 mask = ~0x2000;
        register u16 *flags;
        flags = cache.motionFlags;
        mask &= *flags;
        *flags = mask;
    }
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorMotion_Update);
#endif
