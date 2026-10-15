// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
s32 Mod(s32, s32);
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *);
void SceneSteeringActor_RefreshCollider(struct SceneSteeringActor *);
void sub_0801370C(void *);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
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
        register u32 temp asm("r3");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
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
void SceneSteeringActor_BobInPlace(struct SceneSteeringActorPool *unused,
                                   struct SceneSteeringActor *input)
{
    register struct SceneSteeringActor *p asm("r5") = input;
    {
        u8 *enter = &p->enter;
        if (*enter) {
            register u32 offset asm("r3") = 0x124;
            register u32 zero asm("r4");
            register u16 *frame asm("r1");
            asm("" : "+r"(offset));
            frame = (void *)((u8 *)p + offset);
            zero = 0;
            *frame = 0;
            sub_0801370C(p->shadow);
            *enter = zero;
        }
    }
    {
        struct ContactPosition *v = &p->velocity;
        register u32 zOffset asm("r3");
        v->x = 0;
        zOffset = 0x100;
        *(s16 *)((u8 *)p + zOffset) = 0;
        {
            const s16 *sine = gUnk_086149C4;
            register s32 value asm("r0");
            zOffset += 0x24;
            asm("" : "+r"(zOffset));
            {
                register u8 *phase asm("r0") = (void *)((u8 *)p + zOffset);
                value = sine[*phase] * 4;
            }
            value = Truncate(value);
            p->velocity.y = value;
        }
        {
            struct ContactPosition *left = &p->position;
            {
                register u32 value asm("r0") = (u16)v->x;
                register u32 old asm("r3");
                old = (u16)p->position.x;
                p->position.x = value + old;
            }
            {
                register u32 value asm("r0") = (u16)v->y;
                asm("" : "+r"(value));
                left->y = value + left->y;
            }
            {
                register u32 value asm("r0") = (u16)v->z;
                register u32 old asm("r3");
                old = (u16)left->z;
                left->z = value + old;
            }
            {
                register u32 offset asm("r1") = 0x124;
                {
                    u16 *frame = (void *)((u8 *)p + offset);
                    if (Mod(*frame, 128) == 0)
                        SceneSteeringActor_EmitRandomParticle(left);
                }
            }
        }
    }
    SceneSteeringActor_RefreshCollider(p);
    Step(p);
    {
        register u32 offset asm("r1") = 0x124;
        register u16 *frame asm("r0");
        frame = (void *)((u8 *)p + offset);
        {
            u32 value = *frame;
            u32 mask;
            value += 2;
            mask = 255;
            value &= mask;
            *frame = value;
        }
    }
}
