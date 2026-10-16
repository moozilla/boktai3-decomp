// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *);
/* The final timer load keeps an asm-defined scratch token alive in r1.
 * Only empty-asm inputs consume it; C never uses its value. Preserve its
 * defining output when minimizing hints. The other two loads need clobbers. */
static inline s32 ReadTimer0(s16 *input, u32 offset)
{
    s16 *timer = input;
    register u32 token asm("r1");
    register u32 keep asm("r2") = offset;
    s32 result;
    asm volatile("" : "=r"(token), "+r"(keep));
    result = *timer;
    asm volatile("" : : "r"(token), "r"(keep));
    return result;
}
static inline s32 ReadTimer6(s16 *input, u32 offset)
{
    register s16 *timer asm("r6") = input;
    register u32 keep asm("r2") = offset;
    s32 result;
    asm volatile("" : "+r"(keep)::"r1");
    result = *timer;
    asm volatile("" : : "r"(keep) : "r1");
    return result;
}
static inline s32 ReadTimer1(s16 *input)
{
    register s16 *timer asm("r6") = input;
    s32 result;
    asm volatile("" : ::"r0", "r2");
    result = *timer;
    return result;
}
static inline void Step(struct SceneSteeringActor *input)
{
    register struct SceneSteeringActor *slot asm("r5") = input;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended, duration, flags, bit;
    state = &slot->sequence;
    command = (const volatile u16 *)((u32)slot->sequence.commands + (state->index << 1));
    slot->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));

        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;

            if (first != flip)
                slot->flags |= 4;
            else
                slot->flags &= ~4;
        }
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
            slot->flags |= 8;
        else
            slot->flags &= ~8;
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
        asm("" : "+&r"(mask) : "r"(temp));
        /* Keep the live mask distinct from its source without binding r7. */
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
    return;
}
void SceneSteeringActor_AppearIdle(struct SceneSteeringActorPool *unused,
                                   struct SceneSteeringActor *input)
{
    register struct SceneSteeringActor *p asm("r5") = input;
    u8 *enter = &p->enter;
    if (*enter) {
        p->flags = (p->flags & ~1) | 2;
        {
            register u32 timerOffset asm("r1") = 0x116;
            s16 *timer;
            u32 zeroEnter;
            register u32 zeroFrame asm("r1");
            asm("" : "+r"(timerOffset));
            timer = (void *)((u8 *)p + timerOffset);
            zeroEnter = 0;
            zeroFrame = 0;
            asm("" : "+r"(zeroEnter), "+r"(zeroFrame));
            *timer = 15;
            p->stateFrame = zeroFrame;
            *enter = zeroEnter;
        }
        p->scaleX = 1;
        p->scaleY = 1;
    }
    {
        register u32 offset asm("r1") = 0x124;
        {
            u16 *frame = (void *)((u8 *)p + offset);
            if (Mod(*frame, 8) == 0)
                SceneSteeringActor_EmitRandomParticle(&p->position);
        }
    }
    {
        u32 offset = 0x116;
        register s16 *timer asm("r6");
        timer = (void *)((u8 *)p + offset);
        if (ReadTimer6(timer, offset) > 0) {
            p->scaleX += Div(64 - p->scaleX, *timer);
            p->scaleY += Div(64 - p->scaleY, ReadTimer1(timer));
            (*timer)--;
        }
    }
    Step(p);
    {
        register u32 offset asm("r2") = 0x116;
        s32 value;
        value = ReadTimer0((void *)((u8 *)p + offset), offset);
        if (value <= 0) {
            {
                register u32 stateOffset asm("r0") = 0x122;
                register u8 *state asm("r1");
                state = (void *)((u8 *)p + stateOffset);
                *state = 3;
            }
            p->flags &= ~2;
            {
                register u8 *enter asm("r1");
                offset += 16;
                enter = (void *)((u8 *)p + offset);
                *enter = 1;
            }
        } else {
            register u32 frameOffset asm("r3") = 0x124;
            register u16 *frame asm("r1");
            asm("" : "+r"(frameOffset));
            frame = (void *)((u8 *)p + frameOffset);
            (*frame)++;
        }
    }
}
