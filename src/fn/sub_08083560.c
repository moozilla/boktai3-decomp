#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_motion.h"
void CpuSet(const void *, void *, u32);
#include "gba/macro.h"
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, s32);
static inline u32 StepDrawable(struct SceneActorControlMotionDrawable *owner)
{
    struct SceneActorControlMotionDrawable *sprite = owner;
    struct DisplaySequenceState *state = &owner->sequence;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}
static inline u32 CellIndex(const struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 GroundHeight(u32 cell, const struct ContactPosition *position)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    register u32 flags asm("r1");
    u32 shape, height;
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    height = (flags & 15) << 8;
    switch (shape) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2: {
            register const struct ContactPosition *raw asm("r4") = position;
            asm("" : : "r"(raw));
            height -= *(u8 *)&raw->x;
            break;
        }
    }
    return height;
}
static inline void ClearVector(void *destination, u32 *zero)
{
    void *target = destination;
    u32 *source;
    *zero = 0;
    source = zero;
    CpuSet(source, target, 0x05000002);
}
void SceneActorControl_RestoreMotion(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct SceneActorControlMotionDrawable *drawable = actor->drawable;

    u32 height;
    state->field3DB = 0;
    {
        register u32 offset asm("r4") = 0x3E0;
        u32 zero = 0;
        u8 byteZero = 0;
        register struct ContactPosition *stored asm("r2");
        *(u16 *)((u8 *)state + offset) = zero;
        state->phase = byteZero;
        offset += 3;
        *(s8 *)((u8 *)state + offset) = 1;
        state->sway = zero;
        offset += 3;
        *((u8 *)state + offset) = byteZero;
        state->startTick = byteZero;
        state->flags3D4 = zero;
        offset += 0x4E;
        stored = (void *)((u8 *)state + offset);
        actor->body.position = *stored;
        {
            u32 cell = CellIndex(stored);
            {
                u16 *field = &actor->terrainCell;
                *field = cell;
            }
        }
    }
    height = GroundHeight(actor->terrainCell, &state->storedPosition);
    {
        u16 *ground = &((struct SceneActorControlGroundView *)actor)->ground;
        s16 *actorY;
        u32 previousY;
        u32 difference;
        register u32 offset asm("r4");
        *ground = height;
        actorY = &actor->body.position.y;
        previousY = (u16)*actorY;
        difference = previousY - height;
        offset = 0x2EA;
        asm("" : "+r"(offset));
        {
            register u16 *field asm("r1") = (void *)((u8 *)actor + offset);
            *field = difference;
        }
        *actorY = *ground;
    }
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position = actor->body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->body.position;
    {
        u32 zero;
        {
            register u32 offset asm("r0") = 0x41c;
            asm("" : "+r"(offset));
            ClearVector((u8 *)state + offset, &zero);
        }
        {
            u32 offset = 0x424;
            ClearVector((u8 *)state + offset, &zero);
        }
        {
            u32 offset = 0x42c;
            ClearVector((u8 *)state + offset, &zero);
        }
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepDrawable(drawable);
}
