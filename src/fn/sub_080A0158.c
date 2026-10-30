// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"

struct CrowResetContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 field3D8, unknown3D9[4], field3DD;
    u8 unknown3DE[6];
    u8 field3E4;
    u8 unknown3E5[3];
    u16 cell3E8;
    u8 unknown3EA[4];
    u8 field3EE, field3EF;
    u8 unknown3F0[0x2C];
    struct ContactPosition vectors[3];
    struct ContactPosition storedPosition;
};

struct CrowPlacement {
    u32 xy, z;
};

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

/* Crow ground-state reset (tentative): clears the context's state, restores the stored position,
 * resnaps the actor to the terrain height and steps the display sequence. */
void Crow_ResetGroundState(struct SceneActorRegistryResetView *actor)
{
    struct CrowResetContext *context = actor->context;
    u32 zero;
    context->field3D8 = 0;
    context->field3EE = 0;
    context->field3EF = 0;
    context->field3DD = 0;
    context->field3E4 = 0;
    context->flags3D4 = 0;
    *(struct CrowPlacement *)&actor->body.position = *(struct CrowPlacement *)&context->storedPosition;
    actor->terrainCell = context->cell3E8;
    actor->groundHeight2BE =
        SceneActorTerrain_GetHeight(actor->terrainCell, &context->storedPosition);
    actor->field2EA = actor->body.position.y;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind)
        *(struct CrowPlacement *)((u8 *)actor->drawable + 0x1C) =
            *(struct CrowPlacement *)&actor->body.position;
    else
        *(struct CrowPlacement *)((u8 *)actor->drawable + 0x40) =
            *(struct CrowPlacement *)&actor->body.position;
    {
        void *dst = &context->vectors[0];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    {
        void *dst = &context->vectors[1];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    {
        void *dst = &context->vectors[2];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepSequence(actor->drawable);
}
