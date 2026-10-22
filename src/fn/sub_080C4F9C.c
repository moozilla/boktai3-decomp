#include "global.h"
#include "scene_actor_control_motion.h"
// MATCHING_CONCESSION: register-pinning

struct GolemCombatResetContext {
    u8 pad0[0x624];
    u8 b624;
    u8 pad625[3];
    u32 w628;
    u8 pad62c[0x50];
    u32 w67c;
    u16 h680, h682, h684, h686, h688, h68a;
    u8 b68c, b68d, b68e, b68f, b690, b691;
    u8 pad692[2];
    u8 a694[8];
    u8 a69c[8];
    u8 a6a4[8];
    u16 h6ac, h6ae;
};
struct GolemResetActor {
    u8 pad0[4];
    void *spr;
};

void Memory_Clear(void *, int);
void SceneActorMovement_ResolveRoutePosition(void *);

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

// confidence=low: resets golem combat counters and restarts the idle animation.
void Golem_ResetCombatState(u8 *p)
{
    struct GolemCombatResetContext *in = *(struct GolemCombatResetContext **)(p + 0x3d0);
    in->b624 = 0;
    in->w628 = 0;
    in->w67c = 0;
    in->h682 = 0;
    in->h680 = 0;
    in->h684 = 0;
    in->h686 = 0;
    in->h688 = 0;
    in->h68a = 0;
    in->b68c = 0;
    in->b68d = 0;
    in->b68e = 0;
    in->b68f = 0;
    in->b690 = 0;
    in->b691 = 0;
    Memory_Clear(in->a694, 8);
    Memory_Clear(in->a69c, 8);
    Memory_Clear(in->a6a4, 8);
    Memory_Clear((u8 *)in + 0x674, 8);
    in->h6ac = 0;
    in->h6ae = 0;
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry((void *)p, 0);
    {
        register void *drawable asm("r0") = ((struct GolemResetActor *)p)->spr;
        asm("" : "+r"(drawable));
        StepSequence(drawable);
    }
}
