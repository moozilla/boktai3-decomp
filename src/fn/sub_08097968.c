// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "player.h"

struct IvyPlacement {
    u32 xy, z;
};

static inline void StepSequence(void *obj, u32 one)
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
        register u32 maskBit asm("r8") = one;
        first = maskBit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & maskBit))
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

void CpuSet(const void *, void *, u32);
static inline void Fill(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}
struct GhostMovement {
    u8 pad[0x558];
    u32 flags;
    u8 pad2[0x5B4 - 0x55C];
    u16 time;
    u8 phase, active;
    u16 speed;
    u8 pad3[3];
    u8 a, b;
    u8 pad4[0x5EC - 0x5BF];
    u32 fill[6];
    struct IvyPlacement placement;
};

void Ghost_ResetMovementState(struct SceneActorRegistryResetView *actor)
{
    struct GhostMovement *context = actor->context;
    u32 tmp;
    register u32 one asm("r8");
    context->time = 0;
    context->phase = 0;
    {
        u8 *active = &context->active;
        one = 1;
        *active = one;
    }
    context->speed = 0;
    context->a = 0;
    context->b = 0;
    context->flags = actor->flags130;
    context->placement = *(struct IvyPlacement *)(((u8 **)gPlayers)[0] + 0x30);
    Fill((u8 *)context + 0x5EC, &tmp, 0);
    Fill((u8 *)context + 0x5F4, &tmp, 0);
    Fill((u8 *)context + 0x5FC, &tmp, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        register void *drawable asm("r0") = actor->drawable;
        asm("" : "+r"(drawable));
        StepSequence(drawable, one);
    }
}
