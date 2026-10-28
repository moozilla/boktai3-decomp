// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "player.h"
// confidence=low: positions player before setting release phase
struct Status {
    u8 pad[0x160];
    u32 flags, state, timer;
    u8 a, b, entered, phase, done;
};
void Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32);
static inline void StepSequence(void *obj)
{
    register struct SceneActorControlMotionDrawable *owner asm("r6") = obj;
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

void ActorStatus_BeginPlayerRelease(u8 *ownerArg, struct Status *status)
{
    register struct Status *s asm("r6") = status;
    u8 *owner = ownerArg;
    register u32 mask asm("r2") = 12;
    {
        register u32 *flags asm("r1") = &s->flags;
        *flags |= mask;
    }
    {
        u8 *reset = &s->entered;
        if (*reset) {
            u32 zero = 0;
            *reset = zero;
            s->state = zero;
            s->timer = zero;
            s->phase = 1;
        }
    }
    switch (s->state) {
        case 0:
            Player_ApplyPositionRequest((*(void **)gPlayers), (u8 *)s + 0x1c, 80, 32, 128, 0, 0, 0);
            {
                void *data = *(void **)(owner + 0x4d0);
                register void *seq asm("r4");
                seq = (u8 *)s + 0x48;
                Anim_SetSequence(seq, data, 9, *(u8 *)((u8 *)s + 0x15c), 0);
            }
            StepSequence(s);
            {
                u32 next = 1;
                s->state = next;
                s->timer = 0;
                {
                    register u8 *phase asm("r0") = &s->phase;
                    *phase = next;
                }
            }
            break;
        case 1: {
            register u8 *done asm("r0") = &s->done;
            *done = 1;
        } break;
    }
}
