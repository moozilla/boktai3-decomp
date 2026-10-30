// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_motion.h"
#include "engine.h"
void CpuSet(const void *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(int, int);
struct BeePlacement {
    u32 xy, z;
};
static inline u8 *CellData(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline void StepSequence(void *obj)
{
    struct SceneActorControlMotionDrawable *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = owner->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
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
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
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
                } else {
                    state->index = index - 1;
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
                } else {
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
        }
    }
}

static inline void Clear(u32 *fillp, u32 value, void *out)
{
    *fillp = value;
    CpuSet(fillp, out, 0x05000002);
}
void Bee_ResetRoute(struct SceneActorRegistryResetView *actor)
{
    u32 zero4;
    u8 *bytes;
    u8 *context = actor->context;
    struct ContactPosition *source;
    u32 zero;
    s32 height, slope;
    u8 *c;
    u32 cellIndex;
    u8 zero8;
    u32 fill;
    u8 *first;
    first = context + 0x3DD;
    zero = 0;
    zero8 = 0;
    *first = zero;
    *(u16 *)(context + 0x430) = zero;
    context[0x432] = zero8;
    context[0x433] = 1;
    *(u16 *)(context + 0x434) = zero;
    context[0x436] = zero8;
    context[0x437] = zero8;
    *(u32 *)(context + 0x3D4) = zero;
    source = (struct ContactPosition *)(context + 0x488);
    actor->body.position = *source;
    {
        s32 x = source->x >> 8, z = source->z >> 8, index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        actor->terrainCell = index;
        cellIndex = actor->terrainCell;
    }
    {
        bytes = context + 0x488;
        c = CellData(cellIndex);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= bytes[4];
                break;
            case 2:
                height -= bytes[0];
                break;
        }
    }
    actor->groundHeight2BE = height;
    actor->field2EA = actor->body.position.y - height;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind) {
        *(struct BeePlacement *)((u8 *)actor->drawable + 0x1C) =
            *(struct BeePlacement *)&actor->body.position;
    } else {
        *(struct BeePlacement *)((u8 *)actor->drawable + 0x40) =
            *(struct BeePlacement *)&actor->body.position;
    }
    {
        u8 *dst = context + 0x470;
        zero4 = 0;
        Clear(&fill, zero4, dst);
    }
    {
        u8 *dst = context + 0x478;
        Clear(&fill, zero4, dst);
    }
    {
        u8 *dst = context + 0x480;
        Clear(&fill, zero4, dst);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepSequence(actor->drawable);
}
